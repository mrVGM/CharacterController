const { LoadEJSElement } = require('./loadEJSElement');
const { openModal, closeModal } = require('./modalUtils');

function create(controller, propertiesPanel) {
    const def = controller.def;

    const { buttons } = controller.layout.tagged;

    function selectProp(prop) {
        controller.setSelectedItem({
            itemType: 'property',
            item: prop
        });
    }

    async function choosePropType() {
        const it = document.appData.enumerateDefs();

        let choose;

        const types = [];
        let cur = it.next();
        while (!cur.done) {
            const def = cur.value;

            types.push({
                name: def.name,
                category: def.category,
                chosen: () => {
                    choose(def);
                }
            });

            cur = it.next();
        }

        const pr = new Promise((resolve, reject) => {
            choose = def => {
                resolve(def);
            };
        });

        openModal(types);

        const chosen = await pr;

        closeModal();

        return chosen;
    }

    function addPropEntry(propDef) {
        const slot = propertiesPanel.data.addSlot('Properties/' + propDef.category);
        const prop = LoadEJSElement('property.ejs');

        prop.data.select = () => {
            prop.element.classList.remove('general-row');
            prop.element.classList.add('general-row-selected');
        };

        prop.data.deselect = () => {
            prop.element.classList.remove('general-row-selected');
            prop.element.classList.add('general-row');
        };

        prop.element.addEventListener('click', event => {
            selectProp(prop);
        });

        prop.data.propDef = propDef;
        propertiesPanel.data.addItem(prop, 'New Property', slot.slotId);

        const { name, access, type, secondary_type, structure } = prop.tagged;
        const { type_object_or_def, secondary_type_object_or_def, secondary_type_container } = prop.tagged;

        name.innerHTML = propDef.name;
        access.innerHTML = propDef.access;
        type.innerHTML = document.appData.defs[propDef.type.id].name;
        secondary_type.innerHTML = document.appData.defs[propDef.secondaryType.id].name;
        structure.innerHTML = propDef.structure;

        secondary_type_container.style.display = propDef.structure === 'map' ? '' : 'none';

        type_object_or_def.style.display = document.appData.defs[propDef.type.id].valueType === 'class' ? '' : 'none';
        secondary_type_object_or_def.style.display = document.appData.defs[propDef.secondaryType.id].valueType === 'class' ? '' : 'none';

        type_object_or_def.innerHTML = propDef.type.isObject ? '(object)' : '(def)';
        secondary_type_object_or_def.innerHTML = propDef.secondaryType.isObject ? '(object)' : '(def)';
        type_object_or_def.addEventListener('click', event => {
            propDef.type.isObject = !propDef.type.isObject;
            type_object_or_def.innerHTML = propDef.type.isObject ? '(object)' : '(def)';

            propDef.defaultValue = undefined;
        });
        secondary_type_object_or_def.addEventListener('click', event => {
            propDef.secondaryType.isObject = !propDef.secondaryType.isObject;
            secondary_type_object_or_def.innerHTML = propDef.secondaryType.isObject ? '(object)' : '(def)';

            propDef.defaultValue = undefined;
        });


        function* iterateOptions(options, initial) {
            let i = options.indexOf(initial);

            while (true) {
                i = (i + 1) % options.length;
                yield options[i];
            }
        }

        const accessIt = iterateOptions(['private', 'protected', 'public'], propDef.access);
        const structureIt = iterateOptions(['single', 'array', 'set', 'map'], propDef.structure);


        access.addEventListener('click', event => {
            let newAccess = accessIt.next().value;
            propDef.access = newAccess;
            access.innerHTML = propDef.access;
        });

        structure.addEventListener('click', event => {
            let newStructure = structureIt.next().value;
            propDef.structure = newStructure;
            structure.innerHTML = propDef.structure;

            secondary_type_container.style.display = propDef.structure === 'map' ? '' : 'none';

            propDef.defaultValue = undefined;
            selectProp(prop);
        });

        type.addEventListener('click', async event => {
            const def = await choosePropType();

            propDef.type.id = def.id;
            propDef.type.isObject = def.valueType === 'class';

            type.innerHTML = document.appData.defs[propDef.type.id].name;
            type_object_or_def.style.display = def.valueType === 'class' ? '' : 'none';
            type_object_or_def.innerHTML = propDef.type.isObject ? '(object)' : '(def)';

            propDef.defaultValue = undefined;
            selectProp(prop);

        });

        secondary_type.addEventListener('click', async event => {
            const def = await choosePropType();

            propDef.secondaryType.id = def.id;
            propDef.secondaryType.isObject = def.valueType === 'class';

            secondary_type.innerHTML = document.appData.defs[propDef.secondaryType.id].name;
            secondary_type_object_or_def.style.display = def.valueType === 'class' ? '' : 'none';
            secondary_type_object_or_def.innerHTML = propDef.secondaryType.isObject ? '(object)' : '(def)';

            propDef.defaultValue = undefined;
            selectProp(prop);
        });


        return prop;
    }

    async function createProperty() {
        const chosen = await choosePropType();

        const propDef = {
            id: crypto.randomUUID(),
            name: 'New Property',
            category: '',
            access: 'private',
            type: { id: chosen.id, isObject: chosen.valueType === 'class' ? true : false },
            secondaryType: { id: chosen.id, isObject: chosen.valueType === 'class' ? true : false },
            structure: 'single',
        };

        def.properties[propDef.id] = propDef;

        const newProp = addPropEntry(propDef);
        selectProp(newProp);
    }

    {
        const button = LoadEJSElement('button.ejs');
        const { name } = button.tagged;
        name.innerHTML = "Add Property";

        button.element.addEventListener('click', event => createProperty());

        buttons.appendChild(button.element);
    }

    function refresh(prop) {
        propertiesPanel.data.removeSlot(prop.data.slotId);
        prop.element.remove();

        const newProp = addPropEntry(prop.data.propDef);
        selectProp(newProp);
    }

    function deleteProp(prop) {
        const slotId = prop.data.slotId;
        propertiesPanel.data.removeSlot(slotId);
        prop.element.remove();
        delete def.properties[prop.data.propDef.id];
        controller.setSelectedItem();
    }

    controller.propertiesEditor = {
        refresh: refresh,
        deleteProp: deleteProp,
    };
}

exports.create = create;