const path = require('path');
const fs = require('fs');
const ejs = require('ejs');

const ejsDir = path.join(__dirname, 'ejs');
const defsDir = path.join(__dirname, '../Data/ReflectionData');
const assetsDir = path.join(__dirname, '../Data/Assets');

function readDirectory(dir) {
    const filesData = {};
    function readFile(filePath) {
        let done;

        const pr = new Promise((resolve, reject) => {
            done = () => {
                resolve();
            };
        });

        fs.readFile(filePath, (err, data) => {
            let relativePath = path.relative(ejsDir, filePath);
            relativePath = relativePath.replaceAll('\\', '/');
            filesData[relativePath] = data.toString();
            done();
        });

        return pr;
    }

    async function readFilesInDir(dir) {
        let done;

        const pr = new Promise((resolve, reject) => {
            done = files => {
                resolve(files);
            };
        });

        fs.readdir(dir, (err, files) => {
            done(files);
        });

        const files = await pr;
        return files;
    }

    async function readDir(dir) {
        const files = await readFilesInDir(dir);

        const pending = [];
        files.forEach(fileName => {
            const filePath = path.join(dir, fileName);
            pending.push(readGenericFile(filePath));
        });


        for (let i = 0; i < pending.length; ++i) {
            await pending[i];
        }

        return filesData;
    }

    async function readGenericFile(filePath) {
        let done;

        const pr = new Promise((resolve, reject) => {
            done = () => {
                resolve();
            };
        });

        fs.lstat(filePath, async (err, stats) => {
            if (stats.isFile()) {
                await readFile(filePath);
                done();
                return;
            }
            if (stats.isDirectory()) {
                await readDir(filePath);
                done();
                return;
            }
        });

        return pr;
    }

    return readDir(dir);
}

function readValue(valueNode) {
    const valueType = valueNode.getAttribute('value_type');
    if (valueType === 'bool') {
        const data = parseInt(valueNode.getAttribute('data'));
        return {
            valueType: 'bool',
            data: data
        };
    }

    if (valueType === 'int') {
        const data = parseInt(valueNode.getAttribute('data'));
        return {
            valueType: 'int',
            data: data
        };
    }

    if (valueType === 'float') {
        const data = parseFloat(valueNode.getAttribute('data'));
        return {
            valueType: 'float',
            data: data
        };
    }

    if (valueType === 'string') {
        const data = valueNode.getAttribute('data');
        return {
            valueType: 'string',
            data: data
        };
    }

    if (valueType === 'class') {
        const data = valueNode.getAttribute('data');
        return {
            valueType: 'class',
            data: data
        };
    }

    if (valueType === 'pair') {
        const fst = getDirectChildrenByTagName(valueNode, 'fst')[0];
        const snd = getDirectChildrenByTagName(valueNode, 'snd')[0];

        return {
            valueType: 'pair',
            fst: readValue(fst),
            snd: readValue(snd)
        };
    }
}

function getDirectChildrenByTagName(node, tagName) {
    const tmp = node.getElementsByTagName(tagName);
    const res = [];

    for (let i = 0; i < tmp.length; ++i) {
        const cur = tmp[i];
        if (cur.parentNode === node) {
            res.push(cur);
        }
    }

    return res;
}

function readDefaultValue(defaultValueNode) {
    const id = defaultValueNode.getAttribute('id');
    const structure = defaultValueNode.getAttribute('structure');

    if (structure === 'single') {
        const value = getDirectChildrenByTagName(defaultValueNode, 'value')[0];
        const val = readValue(value);

        return {
            id: id,
            structure: structure,
            data: val
        };
    }

    if (structure === 'array' || structure === 'set' || structure === 'map') {
        const values = getDirectChildrenByTagName(defaultValueNode, 'value');
        const vals = [];
        values.forEach(valueNode => {
            const val = readValue(valueNode);
            vals.push(val);
        });

        return {
            id: id,
            structure: structure,
            data: vals
        };
    }
}

function constructDefs(defs) {
    const parsed = {};

    let bool, int, float, string, type, valueType, referenceType, list;

    for (key in defs) {
        const content = defs[key];
        const json = JSON.parse(content);

        parsed[json.id.id] = json;

        if (json.hint === 'bool') {
            bool = json;
        }

        if (json.hint === 'int') {
            int = json;
        }

        if (json.hint === 'float') {
            float = json;
        }

        if (json.hint === 'string') {
            string = json;
        }

        if (json.hint === 'type') {
            type = json;
        }

        if (json.hint === 'value_type') {
            valueType = json;
        }

        if (json.hint === 'reference_type') {
            referenceType = json;
        }

        if (json.hint === 'list') {
            list = json;
        }
    }

    return {
        parsed,
        bool,
        int,
        float,
        string,
        type,
        valueType,
        referenceType,
        list
    };
}

function serializeDefToXML(id) {
    const def = document.appData.defs[id];
    const src = document.appData.ejsData['def.ejs'];
    let rendered = ejs.render(src, def);
    rendered = rendered.split('\r\n');

    rendered = rendered.filter(line => {
        const index = line.indexOf('<');
        return index >= 0;
    });

    let res = '';
    rendered.forEach(line => {
        res += `${line}
`;
    });

    return res;
}

exports.readDirectory = readDirectory;
exports.constructDefs = constructDefs;
exports.ejsDir = ejsDir;
exports.defsDir = defsDir;
exports.assetsDir = assetsDir;
exports.serializeDefToXML = serializeDefToXML;