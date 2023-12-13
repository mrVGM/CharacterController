const { LoadEJSElement } = require('./loadEJSElement');

function create(controller) {
    const { details } = controller.layout.tagged;

    const detailsPanel = LoadEJSElement('propertyDetailsEditor.ejs');
    details.appendChild(detailsPanel.element);

    const initialDisplay = detailsPanel.element.style.display;

    const { name, change_name, category, default_value_content, delete_button } = detailsPanel.tagged;

    let selected;

    let nameChanged;
    change_name.addEventListener('change', event => {
        const newName = change_name.value.trim();
        if (newName === '') {
            return;
        }
        nameChanged(newName);
    });

    async function rename(propDef) {
        change_name.value = propDef.name;
        name.style.display = 'none';
        change_name.style.display = '';

        const tmp = new Promise((resolve, reject) => {
            nameChanged = newName => {
                nameChanged = undefined;
                resolve(newName);
            };
        });

        const newName = await tmp;
        propDef.name = newName;

        name.style.display = '';
        change_name.style.display = 'none';

        controller.propertiesEditor.refresh(selected.item);
    }
    name.addEventListener('click', event => {
        rename(selected.item.data.propDef);
    });

    category.addEventListener('change', event => {
        if (!selected) {
            return;
        }
        const propDef = selected.item.data.propDef;
        propDef.category = category.value;

        controller.propertiesEditor.refresh(selected.item);
    });

    {
        const button = LoadEJSElement('button.ejs');
        const { name } = button.tagged;
        name.innerHTML = 'Delete';

        delete_button.appendChild(button.element);

        button.element.addEventListener('click', event => {
            controller.propertiesEditor.deleteProp(selected.item);
        });
    }

    function refresh(item) {
        detailsPanel.element.style.display = 'none';
        if (!item) {
            return;
        }

        if (item.itemType !== 'property') {
            return;
        }

        selected = item;
        detailsPanel.element.style.display = initialDisplay;

        const propDef = item.item.data.propDef;
        name.innerHTML = propDef.name;
        category.value = propDef.category;

        default_value_content.innerHTML = '';

        const { create } = require('./propDefaultValueEditor');

        const propDefaultValueEditor = create(propDef);
        default_value_content.appendChild(propDefaultValueEditor.element);
    }

    controller.propertyDetailsEditor = {
        refresh: refresh,
    };
}

exports.create = create;