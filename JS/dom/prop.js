const { debug } = require('console');
const { LoadEJSElement } = require('./loadEJSElement')

function createBoolProp(name, getter) {
    const boolProp = LoadEJSElement('boolProperty.ejs');
    const { name: label, value } = boolProp.tagged;

    label.innerHTML = name;

    return boolProp;
}

function createNumProp(name, getter) {
    const numProp = LoadEJSElement('numProperty.ejs');
    const { name: label, value } = numProp.tagged;

    label.innerHTML = name;

    return numProp;
}

function createStringProp(name, getter) {
    const stringProp = LoadEJSElement('stringProperty.ejs');
    const { name: label, value } = stringProp.tagged;

    label.innerHTML = name;

    return stringProp;
}

function createListProp(type, name, getter) {
    const template = document.appData.defs[type.template];

    const listContainer = LoadEJSElement('listContainer.ejs');
    
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
        const slot = listPanel.data.addSlot('');
        const prop = createProp(template.id, index.toString(), getter);
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
        index = 0;
    }

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

function createProp(propType, name, defaults) {
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
        return createBoolProp(name, defaults);
    }

    if (document.appData.isAssignable(propType.id, int.id.id)) {
        return createNumProp(name, defaults);
    }

    if (document.appData.isAssignable(propType.id, float.id.id)) {
        return createNumProp(name, defaults);
    }

    if (document.appData.isAssignable(propType.id, string.id.id)) {
        return createStringProp(name, defaults);
    }

    if (document.appData.isAssignable(propType.id, list.id.id)) {
        return createListProp(propType, name, defaults);
    }
}

exports.createBoolProp = createBoolProp;
exports.createNumProp = createNumProp;
exports.createStringProp = createStringProp;
exports.createListProp = createListProp;
exports.createProp = createProp;