/* [MS-OLEDS] 2.3.8 CompObjStream */
function parse_compobj(obj) {
	var v = {};
	var o = obj.content;

	/* [MS-OLEDS] 2.3.7 CompObjHeader -- All fields MUST be ignored */
	var l = 28, m;
	m = __lpstr(o, l);
	l += 4 + __readUInt32LE(o,l);
	v.UserType = m;

	/* [MS-OLEDS] 2.3.1 ClipboardFormatOrAnsiString */
	m = __readUInt32LE(o,l); l+= 4;
	switch(m) {
		case 0x00000000: break;
		case 0xffffffff: case 0xfffffffe: l+=4; break;
		default:
			if(m > 0x190) throw new Error("Unsupported Clipboard: " + m.toString(16));
			l += m;
	}

	m = __lpstr(o, l); l += m.length === 0 ? 0 : 5 + m.length; v.Reserved1 = m;

	if((m = __readUInt32LE(o,l)) !== 0x71b2e9f4) return v;
	throw "Unsupported Unicode Extension";
}

/* 2.4.58 Continue logic */
function slurp(R, blob, length, opts) {
	var l = length;
	var bufs = [];
	var d = blob.slice(blob.l,blob.l+l);
	if(opts && opts.enc && opts.enc.insitu_decrypt) switch(R.n) {
	case 'BOF': case 'FilePass': case 'FileLock': case 'InterfaceHdr': case 'RRDInfo': case 'RRDHead': case 'UsrExcl': break;
	default:
		if(d.length === 0) break;
		opts.enc.insitu_decrypt(d);
	}
	bufs.push(d);
	blob.l += l;
	var next = (XLSRecordEnum[__readUInt16LE(blob,blob.l)]);
	while(next != null && next.n === 'Continue') {
		l = __readUInt16LE(blob,blob.l+2);
		bufs.push(blob.slice(blob.l+4,blob.l+4+l));
		blob.l += 4+l;
		next = (XLSRecordEnum[__readUInt16LE(blob, blob.l)]);
	}
	var b = bconcat(bufs);
	prep_blob(b, 0);
	var ll = 0; b.lens = [];
	for(var j = 0; j < bufs.length; ++j) { b.lens.push(ll); ll += bufs[j].length; }
	return R.f(b, b.length, opts);
}

function safe_format_xf(p, opts, date1904) {
	if(!p.XF) return;
	try {
		var fmtid = p.XF.ifmt||0;
		if(p.t === 'e') { p.w = p.w || BErr[p.v]; }
		else if(fmtid === 0) {
			if(p.t === 'n') {
				if((p.v|0) === p.v) p.w = SSF._general_int(p.v);
				else p.w = SSF._general_num(p.v);
			}
			else p.w = SSF._general(p.v);
		}
		else p.w = SSF.format(fmtid,p.v, {date1904:date1904||false});
		if(opts.cellNF) p.z = SSF._table[fmtid];
	} catch(e) { if(opts.WTF) throw e; }
}

function make_cell(val, ixfe, t) {
	return {v:val, ixfe:ixfe, t:t};
}

// 2.3.2
function parse_workbook(blob, options) {
	var wb = {opts:{}};
	var Sheets = {};
	var out = {};
	var Directory = {};
	var found_sheet = false;
	var range = {};
	var last_formula = null;
	var sst = [];
	var cur_sheet = "";
	var Preamble = {};
	var lastcell, last_cell, cc, cmnt, rng, rngC, rngR;
	var shared_formulae = {};
	var array_formulae = []; /* TODO: something more clever */
	var temp_val;
	var country;
	var cell_valid = true;
	var XFs = []; /* XF records */
	var palette = [];
	var get_rgb = function getrgb(icv) {
		if(icv < 8) return XLSIcv[icv];
		if(icv < 64) return palette[icv-8] || XLSIcv[icv];
		return XLSIcv[icv];
	};
	var process_cell_style = function pcs(cell, line) {
		var xfd = line.XF.data;
		if(!xfd || !xfd.patternType) return;
		line.s = {};
		line.s.patternType = xfd.patternType;
		var t;
		if((t = rgb2Hex(get_rgb(xfd.icvFore)))) { line.s.fgColor = {rgb:t}; }
		if((t = rgb2Hex(get_rgb(xfd.icvBack)))) { line.s.bgColor = {rgb:t}; }
	};
	var addcell = function addcell(cell, line, options) {
		if(!cell_valid) return;
		if(options.cellStyles && line.XF && line.XF.data) process_cell_style(cell, line);
		lastcell = cell;
		last_cell = encode_cell(cell);
		if(range.s) {
			if(cell.r < range.s.r) range.s.r = cell.r;
			if(cell.c < range.s.c) range.s.c = cell.c;
		}
		if(range.e) {
			if(cell.r + 1 > range.e.r) range.e.r = cell.r + 1;
			if(cell.c + 1 > range.e.c) range.e.c = cell.c + 1;
		}
		if(options.sheetRows && lastcell.r >= options.sheetRows) cell_valid = false;
		else out[last_cell] = line;
	};
	var opts = {
		enc: false, // encrypted
		sbcch: 0, // cch in the preceding SupBook
		snames: [], // sheetnames
		sharedf: shared_formulae, // shared formulae by address
		arrayf: array_formulae, // array formulae array
		rrtabid: [], // RRTabId
		lastuser: "", // Last User from WriteAccess
		biff: 8, // BIFF version
		codepage: 0, // CP from CodePage record
		winlocked: 0, // fLockWn from WinProtect
		wtf: false
	};
	if(options.password) opts.password = options.password;
	var mergecells = [];
	var objects = [];
	var supbooks = [[]]; // 1-indexed, will hold extern names
	var sbc = 0, sbci = 0, sbcli = 0;
	supbooks.SheetNames = opts.snames;
	supbooks.sharedf = opts.sharedf;
	supbooks.arrayf = opts.arrayf;
	var last_Rn = '';
	var file_depth = 0; /* TODO: make a real stack */

	/* explicit override for some broken writers */
	opts.codepage = 1200;
	set_cp(1200);

	while(blob.l < blob.length - 1) {
		var s = blob.l;
		var RecordType = blob.read_shift(2);
		if(RecordType === 0 && last_Rn === 'EOF') break;
		var length = (blob.l === blob.length ? 0 : blob.read_shift(2)), y;
		var R = XLSRecordEnum[RecordType];
		if(R && R.f) {
			if(options.bookSheets) {
				if(last_Rn === 'BoundSheet8' && R.n !== 'BoundSheet8') break;
			}
			last_Rn = R.n;
			if(R.r === 2 || R.r == 12) {
				var rt = blob.read_shift(2); length -= 2;
				if(!opts.enc && rt !== RecordType) throw "rt mismatch";
				if(R.r == 12){ blob.l += 10; length -= 10; } // skip FRT
			}
			//console.error(R,blob.l,length,blob.length);
			var val;
			if(R.n === 'EOF') val = R.f(blob, length, opts);
			else val = slurp(R, blob, length, opts);
			var Rn = R.n;
			/* BIFF5 overrides */
			if(opts.biff === 5 || opts.biff === 2) switch(Rn) {
				case 'Lbl': Rn = 'Label'; break;
			}
			/* nested switch statements to workaround V8 128 limit */
			switch(Rn) {
				/* Workbook Options */
				case 'Date1904': wb.opts.Date1904 = val; break;
				case 'WriteProtect': wb.opts.WriteProtect = true; break;
				case 'FilePass':
					if(!opts.enc) blob.l = 0;
					opts.enc = val;
					if(opts.WTF) console.error(val);
					if(!options.password) throw new Error("File is password-protected");
					if(val.Type !== 0) throw new Error("Encryption scheme unsupported");
					if(!val.valid) throw new Error("Password is incorrect");
					break;
				case 'WriteAccess': opts.lastuser = val; break;
				case 'FileSharing': break; //TODO
				case 'CodePage':
					/* overrides based on test cases */
					if(val === 0x5212) val = 1200;
					else if(val === 0x8001) val = 1252;
					opts.codepage = val;
					set_cp(val);
					break;
				case 'RRTabId': opts.rrtabid = val; break;
				case 'WinProtect': opts.winlocked = val; break;
				case 'Template': break; // TODO
				case 'RefreshAll': wb.opts.RefreshAll = val; break;
				case 'BookBool': break; // TODO
				case 'UsesELFs': /* if(val) console.error("Unsupported ELFs"); */ break;
				case 'MTRSettings': {
					if(val[0] && val[1]) throw "Unsupported threads: " + val;
				} break; // TODO: actually support threads
				case 'CalcCount': wb.opts.CalcCount = val; break;
				case 'CalcDelta': wb.opts.CalcDelta = val; break;
				case 'CalcIter': wb.opts.CalcIter = val; break;
				case 'CalcMode': wb.opts.CalcMode = val; break;
				case 'CalcPrecision': wb.opts.CalcPrecision = val; break;
				case 'CalcSaveRecalc': wb.opts.CalcSaveRecalc = val; break;
				case 'CalcRefMode': opts.CalcRefMode = val; break; // TODO: implement R1C1
				case 'Uncalced': break;
				case 'ForceFullCalculation': wb.opts.FullCalc = val; break;
				case 'WsBool': break; // TODO
				case 'XF': XFs.push(val); break;
				case 'ExtSST': break; // TODO
				case 'BookExt': break; // TODO
				case 'RichTextStream': break;
				case 'BkHim': break;

				case 'SupBook': supbooks[++sbc] = [val]; sbci = 0; break;
				case 'ExternName': supbooks[sbc][++sbci] = val; break;
				case 'Index': break; // TODO
				case 'Lbl': supbooks[0][++sbcli] = val; break;
				case 'ExternSheet': supbooks[sbc] = supbooks[sbc].concat(val); sbci += val.length; break;

				case 'Protect': out["!protect"] = val; break; /* for sheet or book */
				case 'Password': if(val !== 0 && opts.WTF) console.error("Password verifier: " + val); break;
				case 'Prot4Rev': case 'Prot4RevPass': break; /*TODO: Revision Control*/

				case 'BoundSheet8': {
					Directory[val.pos] = val;
					opts.snames.push(val.name);
				} break;
				case 'EOF': {
					if(--file_depth) break;
					if(range.e) {
						out["!range"] = range;
						if(range.e.r > 0 && range.e.c > 0) {
							range.e.r--; range.e.c--;
							out["!ref"] = encode_range(range);
							range.e.r++; range.e.c++;
						}
						if(mergecells.length > 0) out["!merges"] = mergecells;
						if(objects.length > 0) out["!objects"] = objects;
					}
					if(cur_sheet === "") Preamble = out; else Sheets[cur_sheet] = out;
					out = {};
				} break;
				case 'BOF': {
					if(opts.biff !== 8);
					else if(val.BIFFVer === 0x0500) opts.biff = 5;
					else if(val.BIFFVer === 0x0002) opts.biff = 2;
					else if(val.BIFFVer === 0x0007) opts.biff = 2;
					if(file_depth++) break;
					cell_valid = true;
					out = {};
					if(opts.biff === 2) {
						if(cur_sheet === "") cur_sheet = "Sheet1";
						range = {s:{r:0,c:0},e:{r:0,c:0}};
					}
					else cur_sheet = (Directory[s] || {name:""}).name;
					mergecells = [];
					objects = [];
				} break;
				case 'Number': case 'BIFF2NUM': {
					temp_val = {ixfe: val.ixfe, XF: XFs[val.ixfe], v:val.val, t:'n'};
					if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
					addcell({c:val.c, r:val.r}, temp_val, options);
				} break;
				case 'BoolErr': {
					temp_val = {ixfe: val.ixfe, XF: XFs[val.ixfe], v:val.val, t:val.t};
					if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
					addcell({c:val.c, r:val.r}, temp_val, options);
				} break;
				case 'RK': {
					temp_val = {ixfe: val.ixfe, XF: XFs[val.ixfe], v:val.rknum, t:'n'};
					if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
					addcell({c:val.c, r:val.r}, temp_val, options);
				} break;
				case 'MulRk': {
					for(var j = val.c; j <= val.C; ++j) {
						var ixfe = val.rkrec[j-val.c][0];
						temp_val= {ixfe:ixfe, XF:XFs[ixfe], v:val.rkrec[j-val.c][1], t:'n'};
						if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
						addcell({c:j, r:val.r}, temp_val, options);
					}
				} break;
				case 'Formula': {
					switch(val.val) {
						case 'String': last_formula = val; break;
						case 'Array Formula': throw "Array Formula unsupported";
						default:
							temp_val = {v:val.val, ixfe:val.cell.ixfe, t:val.tt};
							temp_val.XF = XFs[temp_val.ixfe];
							if(options.cellFormula) temp_val.f = "="+stringify_formula(val.formula,range,val.cell,supbooks, opts);
							if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
							addcell(val.cell, temp_val, options);
							last_formula = val;
					}
				} break;
				case 'String': {
					if(last_formula) {
						last_formula.val = val;
						temp_val = {v:last_formula.val, ixfe:last_formula.cell.ixfe, t:'s'};
						temp_val.XF = XFs[temp_val.ixfe];
						if(options.cellFormula) temp_val.f = "="+stringify_formula(last_formula.formula, range, last_formula.cell, supbooks, opts);
						if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
						addcell(last_formula.cell, temp_val, options);
						last_formula = null;
					}
				} break;
				case 'Array': {
					array_formulae.push(val);
				} break;
				case 'ShrFmla': {
					if(!cell_valid) break;
					//if(options.cellFormula) out[last_cell].f = stringify_formula(val[0], range, lastcell, supbooks, opts);
					/* TODO: capture range */
					shared_formulae[encode_cell(last_formula.cell)]= val[0];
				} break;
				case 'LabelSst':
					//temp_val={v:sst[val.isst].t, ixfe:val.ixfe, t:'s'};
					temp_val=make_cell(sst[val.isst].t, val.ixfe, 's');
					temp_val.XF = XFs[temp_val.ixfe];
					if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
					addcell({c:val.c, r:val.r}, temp_val, options);
					break;
				case 'Label': case 'BIFF2STR':
					/* Some writers erroneously write Label */
					temp_val=make_cell(val.val, val.ixfe, 's');
					temp_val.XF = XFs[temp_val.ixfe];
					if(temp_val.XF) safe_format_xf(temp_val, options, wb.opts.Date1904);
					addcell({c:val.c, r:val.r}, temp_val, options);
					break;
				case 'Dimensions': {
					if(file_depth === 1) range = val; /* TODO: stack */
				} break;
				case 'SST': {
					sst = val;
				} break;
				case 'Format': { /* val = [id, fmt] */
					SSF.load(val[1], val[0]);
				} break;

				case 'MergeCells': mergecells = mergecells.concat(val); break;

				case 'Obj': objects[val.cmo[0]] = opts.lastobj = val; break;
				case 'TxO': opts.lastobj.TxO = val; break;

				case 'HLink': {
					for(rngR = val[0].s.r; rngR <= val[0].e.r; ++rngR)
						for(rngC = val[0].s.c; rngC <= val[0].e.c; ++rngC)
							if(out[encode_cell({c:rngC,r:rngR})])
								out[encode_cell({c:rngC,r:rngR})].l = val[1];
				} break;
				case 'HLinkTooltip': {
					for(rngR = val[0].s.r; rngR <= val[0].e.r; ++rngR)
						for(rngC = val[0].s.c; rngC <= val[0].e.c; ++rngC)
							if(out[encode_cell({c:rngC,r:rngR})])
								out[encode_cell({c:rngC,r:rngR})].l.tooltip = val[1];
				} break;

				/* Comments */
				case 'Note': {
					if(opts.biff <= 5 && opts.biff >= 2) break; /* TODO: BIFF5 */
					cc = out[encode_cell(val[0])];
					var noteobj = objects[val[2]];
					if(!cc) break;
					if(!cc.c) cc.c = [];
					cmnt = {a:val[1],t:noteobj.TxO.t};
					cc.c.push(cmnt);
				} break;

				default: switch(R.n) { /* nested */
				case 'ClrtClient': break;
				case 'XFExt': update_xfext(XFs[val.ixfe], val.ext); break;

				case 'NameCmt': break;
				case 'Header': break; // TODO
				case 'Footer': break; // TODO
				case 'HCenter': break; // TODO
				case 'VCenter': break; // TODO
				case 'Pls': break; // TODO
				case 'Setup': break; // TODO
				case 'DefColWidth': break; // TODO
				case 'GCW': break;
				case 'LHRecord': break;
				case 'ColInfo': break; // TODO
				case 'Row': break; // TODO
				case 'DBCell': break; // TODO
				case 'MulBlank': break; // TODO
				case 'EntExU2': break; // TODO
				case 'SxView': break; // TODO
				case 'Sxvd': break; // TODO
				case 'SXVI': break; // TODO
				case 'SXVDEx': break; // TODO
				case 'SxIvd': break; // TODO
				case 'SXDI': break; // TODO
				case 'SXLI': break; // TODO
				case 'SXEx': break; // TODO
				case 'QsiSXTag': break; // TODO
				case 'Selection': break;
				case 'Feat': break;
				case 'FeatHdr': case 'FeatHdr11': break;
				case 'Feature11': case 'Feature12': case 'List12': break;
				case 'Blank': break;
				case 'Country': country = val; break;
				case 'RecalcId': break;
				case 'DefaultRowHeight': case 'DxGCol': break; // TODO: htmlify
				case 'Fbi': case 'Fbi2': case 'GelFrame': break;
				case 'Font': break; // TODO
				case 'XFCRC': break; // TODO
				case 'Style': break; // TODO
				case 'StyleExt': break; // TODO
				case 'Palette': palette = val; break; // TODO
				case 'Theme': break; // TODO
				/* Protection */
				case 'ScenarioProtect': break;
				case 'ObjProtect': break;

				/* Conditional Formatting */
				case 'CondFmt12': break;

				/* Table */
				case 'Table': break; // TODO
				case 'TableStyles': break; // TODO
				case 'TableStyle': break; // TODO
				case 'TableStyleElement': break; // TODO

				/* PivotTable */
				case 'SXStreamID': break; // TODO
				case 'SXVS': break; // TODO
				case 'DConRef': break; // TODO
				case 'SXAddl': break; // TODO
				case 'DConBin': break; // TODO
				case 'DConName': break; // TODO
				case 'SXPI': break; // TODO
				case 'SxFormat': break; // TODO
				case 'SxSelect': break; // TODO
				case 'SxRule': break; // TODO
				case 'SxFilt': break; // TODO
				case 'SxItm': break; // TODO
				case 'SxDXF': break; // TODO

				/* Scenario Manager */
				case 'ScenMan': break;

				/* Data Consolidation */
				case 'DCon': break;

				/* Watched Cell */
				case 'CellWatch': break;

				/* Print Settings */
				case 'PrintRowCol': break;
				case 'PrintGrid': break;
				case 'PrintSize': break;

				case 'XCT': break;
				case 'CRN': break;

				case 'Scl': {
					//console.log("Zoom Level:", val[0]/val[1],val);
				} break;
				case 'SheetExt': {

				} break;
				case 'SheetExtOptional': {

				} break;

				/* VBA */
				case 'ObNoMacros': {

				} break;
				case 'ObProj': {

				} break;
				case 'CodeName': {

				} break;
				case 'GUIDTypeLib': {

				} break;

				case 'WOpt': break; // TODO: WTF?
				case 'PhoneticInfo': break;

				case 'OleObjectSize': break;

				/* Differential Formatting */
				case 'DXF': case 'DXFN': case 'DXFN12': case 'DXFN12List': case 'DXFN12NoCB': break;

				/* Data Validation */
				case 'Dv': case 'DVal': break;

				/* Data Series */
				case 'BRAI': case 'Series': case 'SeriesText': break;

				/* Data Connection */
				case 'DConn': break;
				case 'DbOrParamQry': break;
				case 'DBQueryExt': break;

				/* Formatting */
				case 'IFmtRecord': break;
				case 'CondFmt': case 'CF': case 'CF12': case 'CFEx': break;

				/* Explicitly Ignored */
				case 'Excel9File': break;
				case 'Units': break;
				case 'InterfaceHdr': case 'Mms': case 'InterfaceEnd': case 'DSF': case 'BuiltInFnGroupCount':
				/* View Stuff */
				case 'Window1': case 'Window2': case 'HideObj': case 'GridSet': case 'Guts':
				case 'UserBView': case 'UserSViewBegin': case 'UserSViewEnd':
				case 'Pane': break;
				default: switch(R.n) { /* nested */
				/* Chart */
				case 'Dat':
				case 'Begin': case 'End':
				case 'StartBlock': case 'EndBlock':
				case 'Frame': case 'Area':
				case 'Axis': case 'AxisLine': case 'Tick': break;
				case 'AxesUsed':
				case 'CrtLayout12': case 'CrtLayout12A': case 'CrtLink': case 'CrtLine': case 'CrtMlFrt': case 'CrtMlFrtContinue': break;
				case 'LineFormat': case 'AreaFormat':
				case 'Chart': case 'Chart3d': case 'Chart3DBarShape': case 'ChartFormat': case 'ChartFrtInfo': break;
				case 'PlotArea': case 'PlotGrowth': break;
				case 'SeriesList': case 'SerParent': case 'SerAuxTrend': break;
				case 'DataFormat': case 'SerToCrt': case 'FontX': break;
				case 'CatSerRange': case 'AxcExt': case 'SerFmt': break;
				case 'ShtProps': break;
				case 'DefaultText': case 'Text': case 'CatLab': break;
				case 'DataLabExtContents': break;
				case 'Legend': case 'LegendException': break;
				case 'Pie': case 'Scatter': break;
				case 'PieFormat': case 'MarkerFormat': break;
				case 'StartObject': case 'EndObject': break;
				case 'AlRuns': case 'ObjectLink': break;
				case 'SIIndex': break;
				case 'AttachedLabel': case 'YMult': break;

				/* Chart Group */
				case 'Line': case 'Bar': break;
				case 'Surf': break;

				/* Axis Group */
				case 'AxisParent': break;
				case 'Pos': break;
				case 'ValueRange': break;

				/* Pivot Chart */
				case 'SXViewEx9': break; // TODO
				case 'SXViewLink': break;
				case 'PivotChartBits': break;
				case 'SBaseRef': break;
				case 'TextPropsStream': break;

				/* Chart Misc */
				case 'LnExt': break;
				case 'MkrExt': break;
				case 'CrtCoopt': break;

				/* Query Table */
				case 'Qsi': case 'Qsif': case 'Qsir': case 'QsiSXTag': break;
				case 'TxtQry': break;

				/* Filter */
				case 'FilterMode': break;
				case 'AutoFilter': case 'AutoFilterInfo': break;
				case 'AutoFilter12': break;
				case 'DropDownObjIds': break;
				case 'Sort': break;
				case 'SortData': break;

				/* Drawing */
				case 'ShapePropsStream': break;
				case 'MsoDrawing': case 'MsoDrawingGroup': case 'MsoDrawingSelection': break;
				case 'ImData': break;
				/* Pub Stuff */
				case 'WebPub': case 'AutoWebPub':

				/* Print Stuff */
				case 'RightMargin': case 'LeftMargin': case 'TopMargin': case 'BottomMargin':
				case 'HeaderFooter': case 'HFPicture': case 'PLV':
				case 'HorizontalPageBreaks': case 'VerticalPageBreaks':
				/* Behavioral */
				case 'Backup': case 'CompressPictures': case 'Compat12': break;

				/* Should not Happen */
				case 'Continue': case 'ContinueFrt12': break;

				/* Future Records */
				case 'FrtFontList': case 'FrtWrapper': break;

				/* BIFF5 records */
				case 'ExternCount': break;
				case 'RString': break;
				case 'TabIdConf': case 'Radar': case 'RadarArea': case 'DropBar': case 'Intl': case 'CoordList': case 'SerAuxErrBar': break;

				default: switch(R.n) { /* nested */
				/* Miscellaneous */
				case 'SCENARIO': case 'DConBin': case 'PicF': case 'DataLabExt':
				case 'Lel': case 'BopPop': case 'BopPopCustom': case 'RealTimeData':
				case 'Name': break;
				default: if(options.WTF) throw 'Unrecognized Record ' + R.n;
			}}}}
		} else blob.l += length;
	}
	var sheetnamesraw = opts.biff === 2 ? ['Sheet1'] : Object.keys(Directory).sort(function(a,b) { return Number(a) - Number(b); }).map(function(x){return Directory[x].name;});
	var sheetnames = sheetnamesraw.slice();
	wb.Directory=sheetnamesraw;
	wb.SheetNames=sheetnamesraw;
	if(!options.bookSheets) wb.Sheets=Sheets;
	wb.Preamble=Preamble;
	wb.Strings = sst;
	wb.SSF = SSF.get_table();
	if(opts.enc) wb.Encryption = opts.enc;
	wb.Metadata = {};
	if(country !== undefined) wb.Metadata.Country = country;
	return wb;
}

function parse_xlscfb(cfb, options) {
if(!options) options = {};
fix_read_opts(options);
reset_cp();
var CompObj, Summary, Workbook;
if(cfb.find) {
	CompObj = cfb.find('!CompObj');
	Summary = cfb.find('!SummaryInformation');
	Workbook = cfb.find('/Workbook');
} else {
	prep_blob(cfb, 0);
	Workbook = {content: cfb};
}

if(!Workbook) Workbook = cfb.find('/Book');
var CompObjP, SummaryP, WorkbookP;

if(CompObj) CompObjP = parse_compobj(CompObj);
if(options.bookProps && !options.bookSheets) WorkbookP = {};
else {
	if(Workbook) WorkbookP = parse_workbook(Workbook.content, options, !!Workbook.find);
	else throw new Error("Cannot find Workbook stream");
}

if(cfb.find) parse_props(cfb);

var props = {};
for(var y in cfb.Summary) props[y] = cfb.Summary[y];
for(y in cfb.DocSummary) props[y] = cfb.DocSummary[y];
WorkbookP.Props = WorkbookP.Custprops = props; /* TODO: split up properties */
if(options.bookFiles) WorkbookP.cfb = cfb;
WorkbookP.CompObjP = CompObjP;
return WorkbookP;
}

/* TODO: WTF */
function parse_props(cfb) {
	/* [MS-OSHARED] 2.3.3.2.2 Document Summary Information Property Set */
	var DSI = cfb.find('!DocumentSummaryInformation');
	if(DSI) try { cfb.DocSummary = parse_PropertySetStream(DSI, DocSummaryPIDDSI); } catch(e) {}

	/* [MS-OSHARED] 2.3.3.2.1 Summary Information Property Set*/
	var SI = cfb.find('!SummaryInformation');
	if(SI) try { cfb.Summary = parse_PropertySetStream(SI, SummaryPIDSI); } catch(e) {}
}

