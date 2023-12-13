const { LoadEJSElement } = require('./loadEJSElement');
const modalSpace = document.getElementById('modal_space');

function openModal(entries) {
    const selectionMenu = LoadEJSElement('selectionMenu.ejs');

    modalSpace.style.display = '';
    modalSpace.appendChild(selectionMenu.element);

    const selectionMenuContent = selectionMenu.tagged.selection_menu;

    const { create } = require('./categorizedDataPanel');
    const categorizedDataPanel = create();

    selectionMenuContent.appendChild(categorizedDataPanel.element);

    entries.forEach(entry => {
        const slot = categorizedDataPanel.data.addSlot(entry.category);
        const selectionEntry = LoadEJSElement('selectionEntry.ejs');
        selectionEntry.tagged.name.innerHTML = entry.name;
        categorizedDataPanel.data.addItem(selectionEntry, entry.name, slot.slotId);
        selectionEntry.element.addEventListener('click', event => { entry.chosen(); });
    });
}

function closeModal() {
    modalSpace.innerHTML = '';
    modalSpace.style.display = 'none';
}

exports.openModal = openModal;
exports.closeModal = closeModal;