
/* [MS-XLSB] 2.5.143 */
function parse_StrRun(data, length) {
	return { ich: data.read_shift(2), ifnt: data.read_shift(2) };
}

/* [MS-XLSB] 2.1.7.121 */
function parse_RichStr(data, length) {
	var start = data.l;
	var flags = data.read_shift(1);
	var str = parse_XLWideString(data);
	var rgsStrRun = [];
	var z = { t: str, h: str };
	if((flags & 1) !== 0) { /* fRichStr */
		/* TODO: formatted string */
		var dwSizeStrRun = data.read_shift(4);
		for(var i = 0; i != dwSizeStrRun; ++i) rgsStrRun.push(parse_StrRun(data));
		z.r = rgsStrRun;
	}
	else z.r = "<t>" + escapexml(str) + "</t>";
	if((flags & 2) !== 0) { /* fExtStr */
		/* TODO: phonetic string */
	}
	data.l = start + length;
	return z;
}
function write_RichStr(str, o) {
	/* TODO: formatted string */
	if(o == null) o = new_buf(5+2*str.t.length);
	o.write_shift(1,0);
	write_XLWideString(str.t, o);
	return o;
}

/* [MS-XLSB] 2.5.9 */
function parse_XLSBCell(data) {
	var col = data.read_shift(4);
	var iStyleRef = data.read_shift(2);
	iStyleRef += data.read_shift(1) <<16;
	var fPhShow = data.read_shift(1);
	return { c:col, iStyleRef: iStyleRef };
}
function write_XLSBCell(cell, o) {
	if(o == null) o = new_buf(8);
	o.write_shift(-4, cell.c);
	o.write_shift(3, cell.iStyleRef === undefined ? cell.iStyleRef : cell.s);
	o.write_shift(1, 0); /* fPhShow */
	return o;
}


/* [MS-XLSB] 2.5.21 */
function parse_XLSBCodeName (data, length) { return parse_XLWideString(data, length); }

/* [MS-XLSB] 2.5.166 */
function parse_XLNullableWideString(data) {
	var cchCharacters = data.read_shift(4);
	return cchCharacters === 0 || cchCharacters === 0xFFFFFFFF ? "" : data.read_shift(cchCharacters, 'dbcs');
}
function write_XLNullableWideString(data, o) {
	if(!o) o = new_buf(127);
	o.write_shift(4, data.length > 0 ? data.length : 0xFFFFFFFF);
	if(data.length > 0) o.write_shift(0, data, 'dbcs');
	return o;
}

/* [MS-XLSB] 2.5.168 */
function parse_XLWideString(data) {
	var cchCharacters = data.read_shift(4);
	return cchCharacters === 0 ? "" : data.read_shift(cchCharacters, 'dbcs');
}
function write_XLWideString(data, o) {
	if(o == null) o = new_buf(4+2*data.length);
	o.write_shift(4, data.length);
	if(data.length > 0) o.write_shift(0, data, 'dbcs');
	return o;
}

/* [MS-XLSB] 2.5.114 */
var parse_RelID = parse_XLNullableWideString;
var write_RelID = write_XLNullableWideString;


/* [MS-XLSB] 2.5.122 */
/* [MS-XLS] 2.5.217 */
function parse_RkNumber(data) {
	var b = data.slice(data.l, data.l+4);
	var fX100 = b[0] & 1, fInt = b[0] & 2;
	data.l+=4;
	b[0] &= 0xFC; // b[0] &= ~3;
	var RK = fInt === 0 ? __double([0,0,0,0,b[0],b[1],b[2],b[3]],0) : __readInt32LE(b,0)>>2;
	return fX100 ? RK/100 : RK;
}

/* [MS-XLSB] 2.5.153 */
function parse_UncheckedRfX(data) {
	var cell = {s: {}, e: {}};
	cell.s.r = data.read_shift(4);
	cell.e.r = data.read_shift(4);
	cell.s.c = data.read_shift(4);
	cell.e.c = data.read_shift(4);
	return cell;
}

function write_UncheckedRfX(r, o) {
	if(!o) o = new_buf(16);
	o.write_shift(4, r.s.r);
	o.write_shift(4, r.e.r);
	o.write_shift(4, r.s.c);
	o.write_shift(4, r.e.c);
	return o;
}

/* [MS-XLSB] 2.5.171 */
/* [MS-XLS] 2.5.342 */
function parse_Xnum(data, length) { return data.read_shift(8, 'f'); }
function write_Xnum(data, o) { return (o || new_buf(8)).write_shift(8, 'f', data); }

/* [MS-XLSB] 2.5.198.2 */
var BErr = {
	0x00: "#NULL!",
	0x07: "#DIV/0!",
	0x0F: "#VALUE!",
	0x17: "#REF!",
	0x1D: "#NAME?",
	0x24: "#NUM!",
	0x2A: "#N/A",
	0x2B: "#GETTING_DATA",
	0xFF: "#WTF?"
};
var RBErr = evert_num(BErr);

/* [MS-XLSB] 2.4.321 BrtColor */
function parse_BrtColor(data, length) {
	var out = {};
	var d = data.read_shift(1);
	out.fValidRGB = d & 1;
	out.xColorType = d >>> 1;
	out.index = data.read_shift(1);
	out.nTintAndShade = data.read_shift(2, 'i');
	out.bRed   = data.read_shift(1);
	out.bGreen = data.read_shift(1);
	out.bBlue  = data.read_shift(1);
	out.bAlpha = data.read_shift(1);
}

/* [MS-XLSB] 2.5.52 */
function parse_FontFlags(data, length) {
	var d = data.read_shift(1);
	data.l++;
	var out = {
		fItalic: d & 0x2,
		fStrikeout: d & 0x8,
		fOutline: d & 0x10,
		fShadow: d & 0x20,
		fCondense: d & 0x40,
		fExtend: d & 0x80
	};
	return out;
}
