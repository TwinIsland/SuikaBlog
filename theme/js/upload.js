var setStatus = function (text) {
    document.getElementById('el3').innerText = text;
};

var button = document.getElementById('el2');
button.onclick = function (ev) {
    document.getElementById('el1').click();
};

var sendFileData = function (name, data, chunkSize, authHash) {
    var offset = 0;
    var totalSize = data.length;

    var sendNextChunk = function () {
        var chunk = data.subarray(offset, offset + chunkSize) || '';
        var opts = {
            method: 'POST',
            headers: { 'Suika-Token': authHash },
            body: chunk
        };
        var url = '/api/upload?offset=' + offset;

        setStatus(
            'Uploading ' + name + ', bytes ' + offset + '..' +
            (offset + chunk.length) + ' of ' + totalSize);

        fetch(url, opts)
            .then(function (res) {
                if (res.ok && chunk.length > 0) {
                    offset += chunk.length;
                    if (offset < totalSize) {
                        sendNextChunk();
                    } else {
                        finalizeUpload(authHash, name);
                    }
                } else {
                    res.text().then(function (text) {
                        setStatus('Error: ' + text);
                    });
                }
            })
            .catch(function (error) {
                setStatus('Error: ' + error.message);
            });
    };

    sendNextChunk();
};

var finalizeUpload = function (authHash, name) {
    var url = '/api/upload/finalizer?file=' + encodeURIComponent(name);
    fetch(url, {
        method: 'POST',
        headers: { 'Suika-Token': authHash }
    })
        .then(res => res.json())
        .then(response => {
            if (response.status) {
                setStatus('Upload successful! File available at: ' + response.content);
            } else {
                setStatus('Upload failed: ' + response.content);
            }
        })
        .catch(error => {
            setStatus('Error finalizing upload: ' + error.message);
        });
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
