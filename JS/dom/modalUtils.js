const { LoadEJSElement } = require('./loadEJSElement');
const modalSpace = document.getElementById('modal_space');

function openModal(entries, title) {
    const selectionMenu = LoadEJSElement('selectionMenu.ejs');

    const { header, selection_menu } = selectionMenu.tagged;

    if (header) {
        header.innerHTML = title;
    }

    modalSpace.style.display = '';
    modalSpace.appendChild(selectionMenu.element);

    const { create } = require('./categorizedDataPanel');
    const categorizedDataPanel = create();

    selection_menu.appendChild(categorizedDataPanel.element);

    entries.forEach(entry => {
        const slot = categorizedDataPanel.data.addSlot(entry.category);
        const selectionEntry = LoadEJSElement('selectionEntry.ejs');
        selectionEntry.tagged.name.innerHTML = entry.name;
        categorizedDataPanel.data.addItem(selectionEntry, entry.name, slot.slotId);
        selectionEntry.element.addEventListener('click', event => { entry.chosen(); });
    });
}

function openFileDetailsModal(def) {
    const selectionMenu = LoadEJSElement('selectionMenu.ejs');
    const fileDetails = LoadEJSElement('fileDetails.ejs');
    const okButton = LoadEJSElement('button.ejs');

    let done;
    const { header, selection_menu } = selectionMenu.tagged;
    const { filename, category, ok } = fileDetails.tagged;

    const res = {
        filename: def.name,
        category: def.category
    };

    {
        filename.value = res.filename;
        filename.addEventListener('change', event => {
            res.filename = filename.value.trim();
        });
    }
    {
        category.value = res.category;
        category.addEventListener('change', event => {
            res.category = category.value.trim();
        });
    }

    ok.appendChild(okButton.element);
    {
        const { name } = okButton.tagged;
        name.innerHTML = 'OK';
        okButton.element.addEventListener('click', event => {
            done();
        });
    }

    header.innerHTML = 'Change File Details';
    
    modalSpace.style.display = '';
    modalSpace.appendChild(selectionMenu.element);

    selection_menu.appendChild(fileDetails.element);

    return new Promise((resolve, reject) => {
        done = () => {
            resolve(res);
        };
    });
}

function closeModal() {
    modalSpace.innerHTML = '';
    modalSpace.style.display = 'none';
}

exports.openModal = openModal;
exports.openFileDetailsModal = openFileDetailsModal;
exports.closeModal = closeModal;