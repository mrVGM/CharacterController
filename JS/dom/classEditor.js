const { LoadEJSElement } = require('./loadEJSElement');
const { getTabsController } = require('../controllers/tabsController');

const { create: createClassPropsEditor } = require('./classPropsEditor');
const { create: createClassFuncsEditor } = require('./classFuncsEditor');
const { create: createPropertyDetailsEditor } = require('./propertyDetailsEditor');
const { create: createFuncDetailsEditor } = require('./funcDetailsEditor');
const { create: createClassEditorController } = require('../controllers/classEditorController');
function create(def) {
    const controller = createClassEditorController(def);
    const classEditor = LoadEJSElement('classEditor.ejs');
    controller.layout = classEditor;

    const { properties } = classEditor.tagged;

    const { create: createPropsPanel } = require('./categorizedDataPanel');
    const propsPanel = createPropsPanel();
    properties.appendChild(propsPanel.element);

    createClassPropsEditor(controller, propsPanel);
    createClassFuncsEditor(controller, propsPanel);
    createPropertyDetailsEditor(controller);
    createFuncDetailsEditor(controller);

    controller.setSelectedItem();

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