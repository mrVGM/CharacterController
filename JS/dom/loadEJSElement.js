const ejs = require('ejs');

function LoadEJSElement(ejsFile, data) {
    const src = document.appData.ejsData[ejsFile];
    let rendered = ejs.render(src, data);

    let tmp = document.createElement('template');
    tmp.innerHTML = rendered;

    const elem = tmp.content.firstChild;

    function* query(elem) {
        if (elem.getAttribute) {
            const appId = elem.getAttribute('app-id');
            if (appId !== null) {
                yield {
                    appId: appId,
                    elem: elem
                }
            }
        }

        for (let i = 0; i < elem.childNodes.length; ++i) {
            const child = elem.childNodes[i];
            const it = query(child);

            let cur = it.next();
            while (!cur.done) {
                yield cur.value;
                cur = it.next();
            }
        }
    }

    let res = {
        element: elem,
        data: {},
        tagged: {}
    };

    const it = query(elem);
    let cur = it.next();
    while (!cur.done) {
        let val = cur.value;
        res.tagged[val.appId] = val.elem;

        cur = it.next();
    }

    res.element.ejsData = res;

    {
        function startUpdatingPadding(padded) {
            function getEJSData(elem) {
                if (!elem) {
                    return;
                }

                if (elem.ejsData) {
                    return elem.ejsData;
                }

                return getEJSData(elem.parentElement);
            }

            function* getEJSParentChain(elem) {
                let curEJS = getEJSData(elem);

                while (curEJS) {
                    yield curEJS;
                    curEJS = getEJSData(curEJS.element.parentElement);
                }
            }

            function updatePadding() {
                let level = 0;

                const it = getEJSParentChain(padded);
                let cur = it.next();
                while (!cur.done) {
                    const { padded: tmp } = cur.value.tagged;
                    if (tmp) {
                        ++level;
                    }
                    cur = it.next();
                }

                padded.style['padding-left'] = (Math.max(level - 1, 0) * 30) + 'px';

                setTimeout(updatePadding, 0);
            }

            updatePadding();
        }

        const { padded } = res.tagged;
        if (padded) {
            startUpdatingPadding(padded);
        }
    }

    return res;
}

exports.LoadEJSElement = LoadEJSElement;