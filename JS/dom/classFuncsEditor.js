const { LoadEJSElement } = require('./loadEJSElement');

function create(controller, funcsPanel) {
    const def = controller.def;

    const { buttons } = controller.layout.tagged;

    function selectFunc(func) {
        controller.setSelectedItem({
            itemType: 'function',
            item: func
        });
    }

    function addFuncEntry(funcDef) {
        const slot = funcsPanel.data.addSlot('Functions/' + funcDef.category);
        const func = LoadEJSElement('function.ejs');

        func.data.select = () => {
            func.element.classList.remove('general-row');
            func.element.classList.add('general-row-selected');
        };

        func.data.deselect = () => {
            func.element.classList.remove('general-row-selected');
            func.element.classList.add('general-row');
        };

        func.element.addEventListener('click', event => {
            selectFunc(func);
        });

        func.data.funcDef = funcDef;
        funcsPanel.data.addItem(func, 'New Function', slot.slotId);

        const { name, access, static, purity } = func.tagged;

        name.innerHTML = funcDef.name;
        access.innerHTML = funcDef.access;
        static.innerHTML = funcDef.static;
        purity.innerHTML = funcDef.purity;

        function* iterateOptions(options, initial) {
            let i = options.indexOf(initial);

            while (true) {
                i = (i + 1) % options.length;
                yield options[i];
            }
        }

        const accessIt = iterateOptions(['private', 'protected', 'public'], funcDef.access);
        const staticIt = iterateOptions(['static', 'non-static'], funcDef.static);
        const purityIt = iterateOptions(['pure', 'impure'], funcDef.purity);

        access.addEventListener('click', event => {
            let newAccess = accessIt.next().value;
            funcDef.access = newAccess;
            access.innerHTML = funcDef.access;
        });

        static.addEventListener('click', event => {
            let newStatic = staticIt.next().value;
            funcDef.static = newStatic;
            static.innerHTML = funcDef.static;
            selectFunc(func);
        });

        purity.addEventListener('click', event => {
            let newPurity = purityIt.next().value;
            funcDef.purity = newPurity;
            purity.innerHTML = funcDef.purity;
            selectFunc(func);
        });

        return func;
    }

    async function createFunction() {
        const funcDef = {
            id: crypto.randomUUID(),
            name: 'New Function',
            category: '',
            access: 'private',
            static: 'non-static',
            purity: 'impure',
            inputs: {},
            outputs: {}
        };

        def.funcs[funcDef.id] = funcDef;

        const newFunc = addFuncEntry(funcDef);
        selectFunc(newFunc);
    }

    {
        const button = LoadEJSElement('button.ejs');
        const { name } = button.tagged;
        name.innerHTML = "Add Function";

        button.element.addEventListener('click', event => {
            console.log('dadsad');
            createFunction();
        });

        buttons.appendChild(button.element);
    }

    function deleteFunc(func) {
        const slotId = func.data.slotId;
        funcsPanel.data.removeSlot(slotId);
        func.element.remove();
        delete def.funcs[func.data.funcDef.id];
        controller.setSelectedItem();
    }

    function refresh(func) {
        funcsPanel.data.removeSlot(func.data.slotId);
        func.element.remove();

        const newFunc = addFuncEntry(func.data.funcDef);
        selectFunc(newFunc);
    }

    controller.funcsEditor = {
        refresh: refresh,
        deleteFunc, deleteFunc
    };
}

exports.create = create;