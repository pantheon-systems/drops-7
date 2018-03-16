function getdata(data) {
	if(!data) return null;
	if(data.data) return data.data;
	if(data.asNodeBuffer && has_buf) return data.asNodeBuffer().toString('binary');
	if(data.asBinary) return data.asBinary();
	if(data._data && data._data.getContent) return cc2str(Array.prototype.slice.call(data._data.getContent(),0));
	return null;
}

function safegetzipfile(zip, file) {
	var f = file; if(zip.files[f]) return zip.files[f];
	f = file.toLowerCase(); if(zip.files[f]) return zip.files[f];
	f = f.replace(/\//g,'\\'); if(zip.files[f]) return zip.files[f];
	return null;
}

function getzipfile(zip, file) {
	var o = safegetzipfile(zip, file);
	if(o == null) throw new Error("Cannot find file " + file + " in zip");
	return o;
}

function getzipdata(zip, file, safe) {
	if(!safe) return getdata(getzipfile(zip, file));
	if(!file) return null;
	try { return getzipdata(zip, file); } catch(e) { return null; }
}

var _fs, jszip;
if(typeof JSZip !== 'undefined') jszip = JSZip;
if (typeof exports !== 'undefined') {
	if (typeof module !== 'undefined' && module.exports) {
		if(has_buf && typeof jszip === 'undefined') jszip = require('js'+'zip');
		if(typeof jszip === 'undefined') jszip = require('./js'+'zip').JSZip;
		_fs = require('f'+'s');
	}
}
