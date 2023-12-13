const { LoadEJSElement } = require('./loadEJSElement');
const { openModal, closeModal } = require('./modalUtils');
function create(paramDef, deleteProp) {
    const funcParam = LoadEJSElement('funcParam.ejs');

    const {
        name,
        change_name,
        type,
        type_object_or_def,
        secondary_type_container,
        secondary_type,
        secondary_type_object_or_def,
        structure,
        delete_button,
    } = funcParam.tagged;

    name.innerHTML = paramDef.name;

    {
        const button = LoadEJSElement('button.ejs');
        delete_button.appendChild(button.element);

        const { name } = button.tagged;

        name.innerHTML = 'Delete';

        button.element.addEventListener('click', event => {
            deleteProp();
        });
    }

    function refresh() {
        name.innerHTML = paramDef.name;
        const typeDef = document.appData.defs[paramDef.type.id];
        const secondaryTypeDef = document.appData.defs[paramDef.secondaryType.id];

        type.innerHTML = typeDef.name;
        type_object_or_def.style.display = typeDef.valueType === 'class' ? '' : 'none';
        type_object_or_def.innerHTML = paramDef.type.isObject ? '(object)' : '(def)';

        secondary_type_container.style.display = paramDef.structure === 'map' ? '' : 'none';

        secondary_type.innerHTML = secondaryTypeDef.name;
        secondary_type_object_or_def.style.display = secondaryTypeDef.valueType === 'class' ? '' : 'none';
        secondary_type_object_or_def.innerHTML = paramDef.secondaryType.isObject ? '(object)' : '(def)';

        structure.innerHTML = paramDef.structure;
    }

    refresh();

    function* iterateOptions(options, initial) {
        let i = options.indexOf(initial);

        while (true) {
            i = (i + 1) % options.length;
            yield options[i];
        }
    }

    async function chooseParamType() {
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


    let nameChanged;

    change_name.addEventListener('change', event => {
        const newName = change_name.value.trim();
        if (newName === '') {
            return;
        }

        nameChanged(newName);
    });

    name.addEventListener('click', async event => {
        name.style.display = 'none';
        change_name.style.display = '';
        change_name.value = paramDef.name;

        const tmp = new Promise((resolve, reject) => {
            nameChanged = newName => {
                resolve(newName);
            };
        });

        const newName = await tmp;

        name.style.display = '';
        change_name.style.display = 'none';
        paramDef.name = newName;
        refresh();
    });

    type_object_or_def.addEventListener('click', event => {
        paramDef.type.isObject = !paramDef.type.isObject;
        refresh();
    });

    secondary_type_object_or_def.addEventListener('click', event => {
        paramDef.secondaryType.isObject = !paramDef.secondaryType.isObject;
        refresh();
    });

    const structureIt = iterateOptions(['single', 'array', 'set', 'map'], paramDef.structure);
    structure.addEventListener('click', event => {
        const newStructure = structureIt.next().value;
        paramDef.structure = newStructure;

        refresh();
    });

    type.addEventListener('click', async event => {
        const typeDef = await chooseParamType();

        paramDef.type = {
            id: typeDef.id,
            isObject: typeDef.valueType === 'class'
        };

        refresh();
    });

    secondary_type.addEventListener('click', async event => {
        const typeDef = await chooseParamType();

        paramDef.secondaryType = {
            id: typeDef.id,
            isObject: typeDef.valueType === 'class'
        };

        refresh();
    });

    return funcParam;
}

exports.create = create;