const { LoadEJSElement } = require('./loadEJSElement')

function createBoolProp(name, accessors) {
    const boolProp = LoadEJSElement('boolProperty.ejs');
    const { name: label, value } = boolProp.tagged;

    label.innerHTML = name;

    value.checked = !!accessors.get();
    accessors.set(value.checked ? 1 : 0);

    value.addEventListener('change', event => {
        accessors.set(value.checked ? 1 : 0);
    });

    return boolProp;
}

function createNumProp(name, accessors) {
    const numProp = LoadEJSElement('numProperty.ejs');
    const { name: label, value } = numProp.tagged;

    label.innerHTML = name;

    if (!accessors.get()) {
        accessors.set(0);
    }
    value.value = accessors.get();

    value.addEventListener('change', event => {
        accessors.set(parseFloat(value.value));
    });

    return numProp;
}

function createStringProp(name, accessors) {
    const stringProp = LoadEJSElement('stringProperty.ejs');
    const { name: label, value } = stringProp.tagged;

    label.innerHTML = name;

    if (!accessors.get()) {
        accessors.set('');
    }
    value.value = accessors.get();

    value.addEventListener('change', event => {
        accessors.set(value.value);
    });

    return stringProp;
}

function createListProp(type, name, accessors) {
    const template = document.appData.defs[type.template];

    const listContainer = LoadEJSElement('listContainer.ejs');

    if (!accessors.get()) {
        accessors.set([]);
    }

    const addButton = LoadEJSElement('button.ejs');
    const clearButton = LoadEJSElement('button.ejs');

    {
        const { name: listName, buttons, list_header, content, expand_icon } = listContainer.tagged;
        listName.innerHTML = name;

        buttons.appendChild(addButton.element);
        buttons.appendChild(clearButton.element);

        let expanded = true;
        list_header.addEventListener('click', event => {
            expanded = !expanded;
            content.style.display = expanded ? '' : 'none';

            expand_icon.classList.remove(expanded ? 'expand-button-collapsed' : 'expand-button-expanded');
            expand_icon.classList.add(expanded ? 'expand-button-expanded' : 'expand-button-collapsed');
        });
    }

    const { create: createListPanel } = require('./categorizedDataPanel');
    const listPanel = createListPanel();
    listPanel.data.setSorted(false);
    const { search_box, contents_holder } = listPanel.tagged;

    contents_holder.style.position = '';
    search_box.style.display = 'none';

    const { content } = listContainer.tagged;
    content.appendChild(listPanel.element);

    let index = 0;

    let itemsAdded = [];

    function add() {
        const l = accessors.get();
        if (l.length <= itemsAdded.length) {
            l.push();
        }

        function createAccessors(index) {
            return {
                get: () => l[index],
                set: x => { l[index] = x; }
            };
        }

        const slot = listPanel.data.addSlot('');
        const prop = createProp(template.id, index.toString(), createAccessors(index));
        listPanel.data.addItem(prop, index, slot.slotId);
        itemsAdded.push(prop);
        index++;
    }

    function clear() {
        itemsAdded.forEach(x => {
            listPanel.data.removeSlot(x.data.slotId);
            x.element.remove();
        });

        itemsAdded = [];
        accessors.set([]);
        index = 0;
    }

    accessors.get().forEach(x => {
        add();
    });
    
    {
        const { name } = addButton.tagged;
        name.innerHTML = 'Add';
        addButton.element.addEventListener('click', event => {
            add();
            event.stopPropagation();
        });
    }

    {
        const { name } = clearButton.tagged;
        clearButton.element.style['margin-left'] = '3px';
        name.innerHTML = 'Clear';
        clearButton.element.addEventListener('click', event => {
            clear();
            event.stopPropagation();
        });
    }

    return listContainer;
}

function createTypeProp(type, name, accessors) {
    const template = document.appData.defs[type.template];

    function getVal() {
        let val = accessors.get();
        if (val) {
            val = document.appData.defs[val.id];
        }

        return val;
    }

    const typeProp = LoadEJSElement('typeProperty.ejs');
    const { name: label, type_button } = typeProp.tagged;

    const button = LoadEJSElement("button.ejs");
    type_button.appendChild(button.element);

    {
        const { name: buttonName } = button.tagged;
        {
            const valTmp = getVal();
            buttonName.innerHTML = valTmp ? valTmp.name : 'Choose Type';
        }

        button.element.addEventListener('click', async event => {
            const { openModal, closeModal } = require('./modalUtils');

            const assignable = [];
            const it = document.appData.enumerateAssignable(template);
            let cur = it.next();

            const pr = new Promise((resolve, reject) => {
                while (!cur.done) {
                    const def = cur.value;
                    cur = it.next();

                    assignable.push({
                        name: def.name,
                        category: def.category,
                        chosen: () => {
                            resolve(def);
                        }
                    });
                }
            });

            openModal(assignable, "Choose item:");
            const chosen = await pr;
            accessors.set(chosen.id);
            buttonName.innerHTML = getVal().name;
            closeModal();
        });
    }

    label.innerHTML = name;

    return typeProp;
}
function createProp(propType, name, accessors) {
    const {
        bool,
        int,
        float,
        string,
        type,
        valueType,
        referenceType,
        list
    } = document.appData.specialTypes;

    if (document.appData.isAssignable(propType.id, bool.id.id)) {
        return createBoolProp(name, accessors);
    }

    if (document.appData.isAssignable(propType.id, int.id.id)) {
        return createNumProp(name, accessors);
    }

    if (document.appData.isAssignable(propType.id, float.id.id)) {
        return createNumProp(name, accessors);
    }

    if (document.appData.isAssignable(propType.id, string.id.id)) {
        return createStringProp(name, accessors);
    }

    if (document.appData.isAssignable(propType.id, type.id.id)) {
        return createTypeProp(propType, name, accessors);
    }

    if (document.appData.isAssignable(propType.id, list.id.id)) {
        return createListProp(propType, name, accessors);
    }
}

exports.createBoolProp = createBoolProp;
exports.createNumProp = createNumProp;
exports.createStringProp = createStringProp;
exports.createListProp = createListProp;
exports.createProp = createProp;