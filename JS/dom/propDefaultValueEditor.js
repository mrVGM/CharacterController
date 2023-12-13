const { LoadEJSElement } = require('./loadEJSElement');
const { openModal, closeModal } = require('./modalUtils');

function createSinglePropEditor(propDef, defaultValue) {
    const propValue = LoadEJSElement('propertyValue.ejs');

    async function chooseChildClass(id) {
        const it = document.appData.enumerateAssignableToClass(id);

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

    const { fst, snd } = propValue.tagged;
    {
        const simplePropValue = LoadEJSElement('simplePropertyValue.ejs');
        const { bool, number, string, class_def } = simplePropValue.tagged;

        const type = document.appData.defs[propDef.type.id];
        if (type.isNative && type.valueType === 'bool') {
            bool.style.display = '';

            bool.checked = !!defaultValue.fst;
            bool.addEventListener('change', event => {
                defaultValue.fst = bool.checked;
            });
        }

        if (type.isNative && type.valueType === 'int' || type.valueType === 'float') {
            number.style.display = '';

            if (typeof (defaultValue.fst) !== 'undefined') {
                number.value = parseFloat(defaultValue.fst);
            }

            number.addEventListener('change', event => {
                if (type.valueType === 'int') {
                    defaultValue.fst = parseInt(number.value);
                }

                if (type.valueType === 'float') {
                    defaultValue.fst = parseFloat(number.value);
                }
            });
        }

        if (type.isNative && type.valueType === 'string') {
            string.style.display = '';

            if (typeof (defaultValue.fst) !== 'undefined') {
                string.value = defaultValue.fst;
            }

            string.addEventListener('change', event => {
                defaultValue.fst = string.value;
            });
        }

        if (type.valueType === 'class') {
            class_def.style.display = '';

            if (!propDef.type.isObject) {
                if (defaultValue.fst) {
                    const val = document.appData.defs[defaultValue.fst];
                    class_def.innerHTML = val.name;
                }

                class_def.addEventListener('click', async event => {
                    const chosen = await chooseChildClass(propDef.type.id);
                    defaultValue.fst = chosen.id;

                    class_def.innerHTML = chosen.name;
                });
            }
        }

        fst.appendChild(simplePropValue.element);
    }

    if (propDef.structure === 'map')
    {
        const simplePropValue = LoadEJSElement('simplePropertyValue.ejs');
        const { bool, number, string, class_def } = simplePropValue.tagged;

        const type = document.appData.defs[propDef.secondaryType.id];
        if (type.isNative && type.valueType === 'bool') {
            bool.style.display = '';

            bool.checked = !!defaultValue.snd;
            bool.addEventListener('change', event => {
                defaultValue.snd = bool.checked;
            });
        }

        if (type.isNative && type.valueType === 'int' || type.valueType === 'float') {
            number.style.display = '';

            if (typeof (defaultValue.snd) !== 'undefined') {
                number.value = parseFloat(defaultValue.snd);
            }

            number.addEventListener('change', event => {
                if (type.valueType === 'int') {
                    defaultValue.snd = parseInt(number.value);
                }

                if (type.valueType === 'float') {
                    defaultValue.snd = parseFloat(number.value);
                }
            });
        }

        if (type.isNative && type.valueType === 'string') {
            string.style.display = '';

            if (typeof (defaultValue.snd) !== 'undefined') {
                string.value = defaultValue.snd;
            }

            string.addEventListener('change', event => {
                defaultValue.snd = string.value;
            });
        }

        if (type.valueType === 'class') {
            class_def.style.display = '';

            if (!propDef.secondaryType.isObject) {
                if (defaultValue.snd) {
                    const val = document.appData.defs[defaultValue.snd];
                    class_def.innerHTML = val.name;
                }

                class_def.addEventListener('click', async event => {
                    const chosen = await chooseChildClass(propDef.secondaryType.id);
                    defaultValue.snd = chosen.id;

                    class_def.innerHTML = chosen.name;
                });
            }
        }

        snd.appendChild(simplePropValue.element);
    }


    return propValue;
}

function create(propDef) {
    let defaultValue = propDef.defaultValue;

    if (!defaultValue) {
        if (propDef.structure === 'single') {
            defaultValue = {
                fst: undefined,
                snd: undefined
            };
        }
        else {
            defaultValue = [];
        }
        propDef.defaultValue = defaultValue;
    }

    if (propDef.structure === 'single') {
        const res = createSinglePropEditor(propDef, defaultValue);
        return res;
    }

    const arrayProp = LoadEJSElement('arrayPropDefaultValue.ejs');
    const { header, content } = arrayProp.tagged;

    function addArrayEntry(value) {
        const arrayEntry = LoadEJSElement('arrayPropEntry.ejs');
        const { prop_value, delete_button } = arrayEntry.tagged;

        let val = value;
        if (!val) {
            val = {
                fst: undefined,
                snd: undefined
            };
            defaultValue.push(val);
        }

        const entry = createSinglePropEditor(propDef, val);
        prop_value.appendChild(entry.element);

        const deleteButton = LoadEJSElement('button.ejs');
        {
            const { name } = deleteButton.tagged;
            name.innerHTML = "Delete";
        }
        deleteButton.element.addEventListener('click', event => {
            propDef.defaultValue = defaultValue.filter(x => x !== val);
            arrayEntry.element.remove();
        });

        delete_button.appendChild(deleteButton.element);

        content.appendChild(arrayEntry.element);
    }

    const addButton = LoadEJSElement('button.ejs');
    {
        const { name } = addButton.tagged;
        name.innerHTML = 'Add';
        header.appendChild(addButton.element);
    }

    addButton.element.addEventListener('click', event => {
        addArrayEntry();
    });

    defaultValue.forEach(x => {
        addArrayEntry(x);
    });

    return arrayProp;
}

exports.create = create;