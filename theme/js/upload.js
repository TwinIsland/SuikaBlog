var setStatus = function (text) {
    document.getElementById('el3').innerText = text;
};

var button = document.getElementById('el2');
button.onclick = function (ev) {
    document.getElementById('el1').click();
};

var sendFileData = function (name, data, chunkSize, authHash) {
    var sendChunk = function (offset) {
        var chunk = data.subarray(offset, offset + chunkSize) || '';
        var opts = {
            method: 'POST',
            headers: {'SuikaToken': authHash},
            body: chunk
        };
        var url = '/api/upload?offset=' + offset + '&file=' + encodeURIComponent(name);
        var ok;
        setStatus(
            'Uploading ' + name + ', bytes ' + offset + '..' +
            (offset + chunk.length) + ' of ' + data.length);
        fetch(url, opts)
            .then(function (res) {
                if (res.ok && chunk.length > 0) sendChunk(offset + chunk.length);
                ok = res.ok;
                return res.text();
            })
            .then(function (text) {
                if (!ok) setStatus('Error: ' + text);
            });
    };
    sendChunk(0);
};

var input = document.getElementById('el1');
input.onchange = async function (ev) {
    if (!ev.target.files[0]) return;
    var f = ev.target.files[0], r = new FileReader();
    r.readAsArrayBuffer(f);
    r.onload = async function () {
        var password = document.getElementById('password').value;
        var authHash = await sha256(password);
        ev.target.value = '';
        sendFileData(f.name, new Uint8Array(r.result), 2000000, authHash);
    };
};
