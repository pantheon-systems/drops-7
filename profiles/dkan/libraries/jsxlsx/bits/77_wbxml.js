/* 18.2 Workbook */
var wbnsregex = /<\w+:workbook/;
function parse_wb_xml(data, opts) {
	var wb = { AppVersion:{}, WBProps:{}, WBView:[], Sheets:[], CalcPr:{}, xmlns: "" };
	var pass = false, xmlns = "xmlns";
	data.match(tagregex).forEach(function xml_wb(x) {
		var y = parsexmltag(x);
		switch(strip_ns(y[0])) {
			case '<?xml': break;

			/* 18.2.27 workbook CT_Workbook 1 */
			case '<workbook':
				if(x.match(wbnsregex)) xmlns = "xmlns" + x.match(/<(\w+):/)[1];
				wb.xmlns = y[xmlns];
				break;
			case '</workbook>': break;

			/* 18.2.13 fileVersion CT_FileVersion ? */
			case '<fileVersion': delete y[0]; wb.AppVersion = y; break;
			case '<fileVersion/>': break;

			/* 18.2.12 fileSharing CT_FileSharing ? */
			case '<fileSharing': case '<fileSharing/>': break;

			/* 18.2.28 workbookPr CT_WorkbookPr ? */
			case '<workbookPr': delete y[0]; wb.WBProps = y; break;
			case '<workbookPr/>': delete y[0]; wb.WBProps = y; break;

			/* 18.2.29 workbookProtection CT_WorkbookProtection ? */
			case '<workbookProtection': break;
			case '<workbookProtection/>': break;

			/* 18.2.1  bookViews CT_BookViews ? */
			case '<bookViews>': case '</bookViews>': break;
			/* 18.2.30   workbookView CT_BookView + */
			case '<workbookView': delete y[0]; wb.WBView.push(y); break;

			/* 18.2.20 sheets CT_Sheets 1 */
			case '<sheets>': case '</sheets>': break; // aggregate sheet
			/* 18.2.19   sheet CT_Sheet + */
			case '<sheet': delete y[0]; y.name = utf8read(y.name); wb.Sheets.push(y); break;

			/* 18.2.15 functionGroups CT_FunctionGroups ? */
			case '<functionGroups': case '<functionGroups/>': break;
			/* 18.2.14   functionGroup CT_FunctionGroup + */
			case '<functionGroup': break;

			/* 18.2.9  externalReferences CT_ExternalReferences ? */
			case '<externalReferences': case '</externalReferences>': case '<externalReferences>': break;
			/* 18.2.8    externalReference CT_ExternalReference + */
			case '<externalReference': break;

			/* 18.2.6  definedNames CT_DefinedNames ? */
			case '<definedNames/>': break;
			case '<definedNames>': case '<definedNames': pass=true; break;
			case '</definedNames>': pass=false; break;
			/* 18.2.5    definedName CT_DefinedName + */
			case '<definedName': case '<definedName/>': case '</definedName>': break;

			/* 18.2.2  calcPr CT_CalcPr ? */
			case '<calcPr': delete y[0]; wb.CalcPr = y; break;
			case '<calcPr/>': delete y[0]; wb.CalcPr = y; break;

			/* 18.2.16 oleSize CT_OleSize ? (ref required) */
			case '<oleSize': break;

			/* 18.2.4  customWorkbookViews CT_CustomWorkbookViews ? */
			case '<customWorkbookViews>': case '</customWorkbookViews>': case '<customWorkbookViews': break;
			/* 18.2.3    customWorkbookView CT_CustomWorkbookView + */
			case '<customWorkbookView': case '</customWorkbookView>': break;

			/* 18.2.18 pivotCaches CT_PivotCaches ? */
			case '<pivotCaches>': case '</pivotCaches>': case '<pivotCaches': break;
			/* 18.2.17 pivotCache CT_PivotCache ? */
			case '<pivotCache': break;

			/* 18.2.21 smartTagPr CT_SmartTagPr ? */
			case '<smartTagPr': case '<smartTagPr/>': break;

			/* 18.2.23 smartTagTypes CT_SmartTagTypes ? */
			case '<smartTagTypes': case '<smartTagTypes>': case '</smartTagTypes>': break;
			/* 18.2.22   smartTagType CT_SmartTagType ? */
			case '<smartTagType': break;

			/* 18.2.24 webPublishing CT_WebPublishing ? */
			case '<webPublishing': case '<webPublishing/>': break;

			/* 18.2.11 fileRecoveryPr CT_FileRecoveryPr ? */
			case '<fileRecoveryPr': case '<fileRecoveryPr/>': break;

			/* 18.2.26 webPublishObjects CT_WebPublishObjects ? */
			case '<webPublishObjects>': case '<webPublishObjects': case '</webPublishObjects>': break;
			/* 18.2.25 webPublishObject CT_WebPublishObject ? */
			case '<webPublishObject': break;

			/* 18.2.10 extLst CT_ExtensionList ? */
			case '<extLst>': case '</extLst>': case '<extLst/>': break;
			/* 18.2.7    ext CT_Extension + */
			case '<ext': pass=true; break; //TODO: check with versions of excel
			case '</ext>': pass=false; break;

			/* Others */
			case '<ArchID': break;
			case '<AlternateContent': pass=true; break;
			case '</AlternateContent>': pass=false; break;

			default: if(!pass && opts.WTF) throw 'unrecognized ' + y[0] + ' in workbook';
		}
	});
	if(XMLNS.main.indexOf(wb.xmlns) === -1) throw new Error("Unknown Namespace: " + wb.xmlns);

	parse_wb_defaults(wb);

	return wb;
}

var WB_XML_ROOT = writextag('workbook', null, {
	'xmlns': XMLNS.main[0],
	//'xmlns:mx': XMLNS.mx,
	//'xmlns:s': XMLNS.main[0],
	'xmlns:r': XMLNS.r
});

function safe1904(wb) {
	/* TODO: store date1904 somewhere else */
	try { return parsexmlbool(wb.Workbook.WBProps.date1904) ? "true" : "false"; } catch(e) { return "false"; }
}

function write_wb_xml(wb, opts) {
	var o = [XML_HEADER];
	o[o.length] = WB_XML_ROOT;
	o[o.length] = (writextag('workbookPr', null, {date1904:safe1904(wb)}));
	o[o.length] = "<sheets>";
	for(var i = 0; i != wb.SheetNames.length; ++i)
		o[o.length] = (writextag('sheet',null,{name:wb.SheetNames[i].substr(0,31), sheetId:""+(i+1), "r:id":"rId"+(i+1)}));
	o[o.length] = "</sheets>";
	if(o.length>2){ o[o.length] = '</workbook>'; o[1]=o[1].replace("/>",">"); }
	return o.join("");
}
