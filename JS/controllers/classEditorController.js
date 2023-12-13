function create(def) {
    let selectedItem;
    const res = {
        def: def,
        setSelectedItem: (item) => {
            if (selectedItem) {
                selectedItem.item.data.deselect();
            }
            selectedItem = item;
            if (selectedItem) {
                selectedItem.item.data.select();
            }

            res.propertyDetailsEditor.refresh(selectedItem);
            res.funcDetailsEditor.refresh(selectedItem);
        }
    };

    return res;
}

exports.create = create;