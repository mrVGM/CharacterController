const { LoadEJSElement } = require('./loadEJSElement');

const { getTabsController } = require('../controllers/tabsController');

function createContentBrowser() {
    const contentBrowser = LoadEJSElement('contentBrowser.ejs');

    const { buttons, files } = contentBrowser.tagged;

    const addClassButton = LoadEJSElement('button.ejs');
    const detailsButton = LoadEJSElement('button.ejs');
    const deleteButton = LoadEJSElement('button.ejs');
    const saveAssetsButton = LoadEJSElement('button.ejs');
    {
        const { name } = addClassButton.tagged;
        name.innerHTML = "Add Class";
    }
    {
        const { name } = detailsButton.tagged;
        name.innerHTML = "Details";
    }
    {
        const { name } = deleteButton.tagged;
        name.innerHTML = "Delete";
    }

    {
        const { name } = saveAssetsButton.tagged;
        name.innerHTML = "Save Assets";
    }

    buttons.appendChild(addClassButton.element);
    buttons.appendChild(detailsButton.element);
    buttons.appendChild(deleteButton.element);
    buttons.appendChild(saveAssetsButton.element);

    detailsButton.element.addEventListener('click', async event => {
        if (!selectedFile) {
            return;
        }
        const file = selectedFile;

        const def = selectedFile.data.def;
        if (!def.isGenerated) {
            return;
        }

        const { openFileDetailsModal, closeModal } = require('./modalUtils');

        const { filename, category } = await openFileDetailsModal(def);
        def.name = filename === '' ? def.name : filename;
        def.category = category;

        filesPanel.data.removeSlot(file.data.slotId);
        file.element.remove();
        const { click } = addFileEntry(def);
        click();

        closeModal();
    });

    saveAssetsButton.element.addEventListener('click', event => {
        document.appData.saveAssets();
    });

    deleteButton.element.addEventListener('click', async event => {
        if (!selectedFile) {
            return;
        }

        const def = selectedFile.data.def;
        if (!def.isGenerated) {
            return;
        }

        selectedFile.element.remove();
        filesPanel.data.removeSlot(selectedFile.data.slotId);

        delete document.appData.defs[def.id.id];
        selectedFile = undefined;

        const assetName = def.id.id + '.json';
        await document.appData.moveAssetToTrash(assetName);
        console.log(`Asset ${assetName} moved to trash!`);
    });

    addClassButton.element.addEventListener('click', async event => {
        const { openModal, closeModal } = require('./modalUtils');
        const it = document.appData.enumerateReferenced();

        let choose;

        const classes = [];
        let cur = it.next();
        while (!cur.done) {
            const def = cur.value;
            cur = it.next();

            if (def.isGenerated) {
                continue;
            }

            classes.push({
                name: def.name,
                category: def.category,
                chosen: () => {
                    choose(def);
                }
            });
        }

        const pr = new Promise((resolve, reject) => {
            choose = def => {
                resolve(def);
            };
        });

        openModal(classes, 'Choose Parent Class');

        const chosen = await pr;

        const newDef = {
            id: { id: crypto.randomUUID() },
            parent: chosen.id,
            name: "New Asset",
            category: "",
            isGenerated: 1,
            defaults: {}
        };

        document.appData.defs[newDef.id.id] = newDef;

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

        let fileOpened = false;
        let tabButton;

        file.element.addEventListener('dblclick', event => {
            if (!file.data.def.isGenerated) {
                return;
            }

            const { activateTab, removeTab } = getTabsController();
            if (fileOpened) {
                activateTab(tabButton.data.id);
                return;
            }

            const { create } = require('./classEditor');
            tabButton = create(def);
            activateTab(tabButton.data.id);

            tabButton.element.addEventListener('click', event => {
                activateTab(tabButton.data.id);
            });

            tabButton.element.addEventListener('auxclick', event => {
                if (event.button !== 1) {
                    return;
                }
                tabButton.data.remove();
                fileOpened = false;
                tabButton = undefined;
            });

            fileOpened = true;
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