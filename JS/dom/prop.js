const { LoadEJSElement } = require('./loadEJSElement')

function createBoolProp(propDef, defaults) {
    const boolProp = LoadEJSElement('boolProperty.ejs');
    const { name, value } = boolProp.tagged;

    name.innerHTML = propDef.name;

    return boolProp;
}

function createNumProp(propDef, defaults) {
    const numProp = LoadEJSElement('numProperty.ejs');
    const { name, value } = numProp.tagged;

    name.innerHTML = propDef.name;

    return numProp;
}

function createStringProp(propDef, defaults) {
    const stringProp = LoadEJSElement('stringProperty.ejs');
    const { name, value } = stringProp.tagged;

    name.innerHTML = propDef.name;

    return stringProp;
}

function createListProp(propDef, defaults) {
    const listProperty = LoadEJSElement('listProperty.ejs');
    
    const addButton = LoadEJSElement('button.ejs');
    const clearButton = LoadEJSElement('button.ejs');

    {
        const { name } = addButton.tagged;
        name.innerHTML = 'Add';
    }

    {
        const { name } = clearButton.tagged;
        clearButton.element.style['margin-left'] = '3px';
        name.innerHTML = 'Clear';
    }

    {
        const { name, buttons } = listProperty.tagged;
        name.innerHTML = propDef.name;
        buttons.appendChild(addButton.element);
        buttons.appendChild(clearButton.element);
    }
    return listProperty;
}

function createProp(propDef, defaults) {
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

    if (document.appData.isAssignable(propDef.type.id, bool.id.id)) {
        return createBoolProp(propDef, defaults);
    }

    if (document.appData.isAssignable(propDef.type.id, int.id.id)) {
        return createNumProp(propDef, defaults);
    }

    if (document.appData.isAssignable(propDef.type.id, float.id.id)) {
        return createNumProp(propDef, defaults);
    }

    if (document.appData.isAssignable(propDef.type.id, string.id.id)) {
        return createStringProp(propDef, defaults);
    }

    if (document.appData.isAssignable(propDef.type.id, list.id.id)) {
        return createListProp(propDef, defaults);
    }
}

exports.createBoolProp = createBoolProp;
exports.createNumProp = createNumProp;
exports.createStringProp = createStringProp;
exports.createListProp = createListProp;
exports.createProp = createProp;