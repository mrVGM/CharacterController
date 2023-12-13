const { LoadEJSElement } = require('./loadEJSElement');
const { openModal, closeModal } = require('./modalUtils');
function create(controller) {
    const { details } = controller.layout.tagged;

    const detailsPanel = LoadEJSElement('funcDetailsEditor.ejs');
    details.appendChild(detailsPanel.element);

    const initialDisplay = detailsPanel.element.style.display;

    const {
        name,
        change_name,
        category,
        add_params_buttons,
        definition_inputs,
        definition_outputs,
        delete_button
    } = detailsPanel.tagged;

    let selected;

    {
        const button = LoadEJSElement('button.ejs');
        const { name } = button.tagged;
        name.innerHTML = 'Delete';

        delete_button.appendChild(button.element);

        button.element.addEventListener('click', event => {
            controller.funcsEditor.deleteFunc(selected.item);
        });
    }

    let nameChanged;
    change_name.addEventListener('change', event => {
        const newName = change_name.value.trim();
        if (newName === '') {
            return;
        }
        nameChanged(newName);
    });

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

    {
        const addInput = LoadEJSElement('button.ejs');
        const { name } = addInput.tagged;
        name.innerHTML = 'Add Input';

        add_params_buttons.appendChild(addInput.element);

        addInput.element.addEventListener('click', async event => {
            const type = await chooseParamType();

            const paramDef = {
                id: crypto.randomUUID(),
                name: 'Func Param',
                type: {
                    id: type.id,
                    isObject: type.valueType === 'class'
                },
                secondaryType: {
                    id: type.id,
                    isObject: type.valueType === 'class'
                },
                structure: 'single'
            };

            selected.item.data.funcDef.inputs[paramDef.id] = paramDef;
            refresh(selected);
        });
    }

    {
        const addOutput = LoadEJSElement('button.ejs');
        const { name } = addOutput.tagged;
        name.innerHTML = 'Add Output';

        add_params_buttons.appendChild(addOutput.element);

        addOutput.element.addEventListener('click', async event => {
            const type = await chooseParamType();

            const paramDef = {
                id: crypto.randomUUID(),
                name: 'Func Param',
                type: {
                    id: type.id,
                    isObject: type.valueType === 'class'
                },
                secondaryType: {
                    id: type.id,
                    isObject: type.valueType === 'class'
                },
                structure: 'single'
            };

            selected.item.data.funcDef.outputs[paramDef.id] = paramDef;
            refresh(selected);
        });
    }

    async function rename(funcDef) {
        change_name.value = funcDef.name;
        name.style.display = 'none';
        change_name.style.display = '';

        const tmp = new Promise((resolve, reject) => {
            nameChanged = newName => {
                nameChanged = undefined;
                resolve(newName);
            };
        });

        const newName = await tmp;
        funcDef.name = newName;

        name.style.display = '';
        change_name.style.display = 'none';

        controller.funcsEditor.refresh(selected.item);
    }
    name.addEventListener('click', event => {
        rename(selected.item.data.funcDef);
    });

    category.addEventListener('change', event => {
        if (!selected) {
            return;
        }
        const funcDef = selected.item.data.funcDef;
        funcDef.category = category.value;

        controller.funcsEditor.refresh(selected.item);
    });

    function refresh(item) {
        detailsPanel.element.style.display = 'none';
        if (!item) {
            return;
        }
        if (item.itemType !== 'function') {
            return;
        }
        debugger;

        selected = item;
        detailsPanel.element.style.display = initialDisplay;

        const funcDef = item.item.data.funcDef;
        name.innerHTML = funcDef.name;
        category.value = funcDef.category;

        definition_inputs.innerHTML = '';
        definition_outputs.innerHTML = '';

        for (let key in funcDef.inputs) {
            const paramDef = funcDef.inputs[key];
            const { create } = require('./funcParam');
            const newParam = create(paramDef, () => {
                delete funcDef.inputs[paramDef.id];
                refresh(selected);
            });

            definition_inputs.appendChild(newParam.element);
        }

        for (let key in funcDef.outputs) {
            const paramDef = funcDef.outputs[key];
            const { create } = require('./funcParam');
            const newParam = create(paramDef, () => {
                delete funcDef.outputs[paramDef.id];
                refresh(selected);
            });

            definition_outputs.appendChild(newParam.element);
        }
    }

    controller.funcDetailsEditor = {
        refresh: refresh,
    };
}

exports.create = create;