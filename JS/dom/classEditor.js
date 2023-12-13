const { LoadEJSElement } = require('./loadEJSElement');
const { getTabsController } = require('../controllers/tabsController');

function create(def) {
    debugger;
    const classEditor = LoadEJSElement('classEditor.ejs');
    const { properties } = classEditor.tagged;

    const { create: createPropsPanel } = require('./categorizedDataPanel');
    const propsPanel = createPropsPanel();
    properties.appendChild(propsPanel.element);

    const initialDisplayStyle = classEditor.element.style.display;
    classEditor.element.style.display = 'none';

    const content = document.getElementById('content');
    content.appendChild(classEditor.element);

    const tabButton = LoadEJSElement('tabButton.ejs');
    const { addTab, activateTab } = getTabsController();

    addTab(tabButton);

    const { name } = tabButton.tagged;
    name.innerHTML = def.name;

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

    return tabButton;
}

exports.create = create;