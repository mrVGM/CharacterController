const { LoadEJSElement } = require('./loadEJSElement');
const { getTabsController } = require('../controllers/tabsController');

const propUtils = require('./prop');

function getAllProps(typeId)
{
    let cur = document.appData.defs[typeId];

    const res = {};
    while (cur) {
        for (let p in cur.properties) {
            res[p] = cur.properties[p];
        }

        if (!cur.parent) {
            break;
        }
        cur = document.appData.defs[cur.parent.id];
    }

    return res;
}

function create(def) {
    const props = getAllProps(def.id.id);

    const { defaults } = def;

    const classEditor = LoadEJSElement('classEditor.ejs');
    const { properties } = classEditor.tagged;

    const { create: createPropsPanel } = require('./categorizedDataPanel');
    const propsPanel = createPropsPanel();
    properties.appendChild(propsPanel.element);


    for (let k in props) {
        const cur = props[k];
        const slot = propsPanel.data.addSlot(cur.category);

        const { createProp } = propUtils;
        const prop = createProp(cur.type, cur.name, {
            get: () => defaults[k],
            set: value => { defaults[k] = value; }
        });
        propsPanel.data.addItem(prop, cur.name, slot.slotId);
    }

    const initialDisplayStyle = classEditor.element.style.display;
    classEditor.element.style.display = 'none';

    const content = document.getElementById('content');
    content.appendChild(classEditor.element);

    const tabButton = LoadEJSElement('tabButton.ejs');
    const { addTab, removeTab, activateTab } = getTabsController();

    addTab(tabButton);

    function updateName() {
        if (!tabButton.element.isConnected) {
            return;
        }
        const { name } = tabButton.tagged;
        name.innerHTML = def.name;

        setTimeout(updateName, 500);
    }

    updateName();

    function activate() {
        tabButton.element.classList.remove('tab-button-idle');
        tabButton.element.classList.add('tab-button-selected');

        classEditor.element.style.display = initialDisplayStyle;
    }

    function deactivate() {
        tabButton.element.classList.remove('tab-button-selected');
        tabButton.element.classList.add('tab-button-idle');

        classEditor.element.style.display = 'none';
    }

    tabButton.data.activate = activate;
    tabButton.data.deactivate = deactivate;

    tabButton.element.addEventListener('click', event => {
        activateTab(tabButton.data.id);
    });

    tabButton.element.addEventListener('auxclick', event => {
        if (event.button !== 1) {
            return;
        }
        removeTab(tabButton);
        classEditor.element.remove();
    });

    return tabButton;
}

exports.create = create;