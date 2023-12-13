const { LoadEJSElement } = require('./loadEJSElement');

const { getTabsController } = require('../controllers/tabsController');

function createContentBrowser() {
    const contentBrowser = LoadEJSElement('contentBrowser.ejs');

    const { buttons, files } = contentBrowser.tagged;

    const addClassButton = LoadEJSElement('button.ejs');
    const deleteButton = LoadEJSElement('button.ejs');
    {
        const { name } = addClassButton.tagged;
        name.innerHTML = "Add Class";
    }
    {
        const { name } = deleteButton.tagged;
        name.innerHTML = "Delete";
    }

    buttons.appendChild(addClassButton.element);
    buttons.appendChild(deleteButton.element);

    deleteButton.element.addEventListener('click', event => {
        if (!selectedFile) {
            return;
        }

        const def = selectedFile.data.def;
        if (def.isNative) {
            return;
        }

        selectedFile.element.remove();
        filesPanel.data.removeSlot(selectedFile.data.slotId);

        delete document.appData.defs[def.id];
        selectedFile = undefined;
    });

    addClassButton.element.addEventListener('click', async event => {
        const { openModal, closeModal } = require('./modalUtils');
        const it = document.appData.enumerateReferenced();

        let choose;

        const classes = [];
        let cur = it.next();
        while (!cur.done) {
            const def = cur.value;
            classes.push({
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

        openModal(classes);

        const chosen = await pr;

        const newDef = {
            id: { id: crypto.randomUUID() },
            parent: chosen.id,
            name: "New Asset",
            category: "",
            isGenerated: true,
            defaults: {}
        };

        document.appData.defs[newDef.id] = newDef;

        const { click } = addFileEntry(newDef);
        click();

        closeModal();
    });

    const { create: createFilesPanel } = require('./categorizedDataPanel');
    const filesPanel = createFilesPanel();
    files.appendChild(filesPanel.element);
    
    let selectedFile;
    
    function addFileEntry(def) {
        const cat = (def.isGenerated ? 'Generated/' : 'Native/') + def.category;
        const slot = filesPanel.data.addSlot(cat);

        const file = LoadEJSElement('fileEntry.ejs');
        file.data.def = def;

        let defName;
        {
            const { name } = file.tagged;
            defName = def.name;
            if (defName === '') {
                defName = def.id.id;
            }
            name.innerHTML = defName;
        }

        filesPanel.data.addItem(file, defName, slot.slotId);

        file.data.select = () => {
            file.element.classList.remove("general-row");
            file.element.classList.add("general-row-selected");
        };

        file.data.deselect = () => {
            file.element.classList.remove("general-row-selected");
            file.element.classList.add("general-row");
        };

        function click() {
            if (selectedFile === file) {
                return;
            }

            if (selectedFile) {
                selectedFile.data.deselect();
            }

            selectedFile = file;
            selectedFile.data.select();
        }

        file.element.addEventListener('click', event => {
            click();
        });

        file.element.addEventListener('dblclick', event => {
            if (!file.data.def.isGenerated) {
                return;
            }

            const { create } = require('./classEditor');
            const tabButton = create(def);

            const tabsController = getTabsController();
            tabsController.activateTab(tabButton.data.id);
        });

        return {
            click: click,
        }
    }

    const it = document.appData.enumerateDefs();
    let cur = it.next();
    while (!cur.done) {
        const def = cur.value;
        
        addFileEntry(def);

        cur = it.next();
    }

    return contentBrowser;
}

function create() {
    const contentBrowser = createContentBrowser();

    const initialDisplayStyle = contentBrowser.element.style.display;
    contentBrowser.element.style.display = 'none';

    const content = document.getElementById('content');
    content.appendChild(contentBrowser.element);

    const tabButton = LoadEJSElement('tabButton.ejs');
    const { addTab, activateTab } = getTabsController();

    addTab(tabButton);

    const { name } = tabButton.tagged;
    name.innerHTML = 'Content Browser';

    function activate() {
        tabButton.element.classList.remove('tab-button-idle');
        tabButton.element.classList.add('tab-button-selected');

        contentBrowser.element.style.display = initialDisplayStyle;
    }

    function deactivate() {
        tabButton.element.classList.remove('tab-button-selected');
        tabButton.element.classList.add('tab-button-idle');

        contentBrowser.element.style.display = 'none';
    }

    tabButton.data.activate = activate;
    tabButton.data.deactivate = deactivate;

    tabButton.element.addEventListener('click', event => {
        activateTab(tabButton.data.id);
    });
}

exports.create = create;