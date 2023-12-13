let tabsController;

function create() {
    let buttonId = 0;
    const buttons = {};
    let active = -1;

    function addTab(button) {
        button.data.id = buttonId++;

        const header = document.getElementById('header');
        header.appendChild(button.element);

        buttons[button.data.id] = button;
    }

    function activateTab(buttonId) {
        if (active === buttonId) {
            return;
        }

        if (active >= 0) {
            const cur = buttons[active];
            cur.data.deactivate();
        }

        const tab = buttons[buttonId];
        tab.data.activate();

        active = buttonId;
    }

    const res = {
        addTab: addTab,
        activateTab: activateTab
    };

    return res;
}

function getTabsController() {
    if (!tabsController) {
        tabsController = create();
    }

    return tabsController;
}

exports.getTabsController = getTabsController;