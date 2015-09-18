A simple javascript library that wraps the Google Docs JSON API. Part of the
[Recline][] suite of data libraries (but has *no* dependencies on core
Recline).

[Recline]: http://okfnlabs.org/recline/

## Usage

Get data from the API:

    recline.Backend.GDocs.fetch({
      url: 'https://docs.google.com/a/okfn.org/spreadsheet/ccc?key=0Aon3JiuouxLUdDlGV2lCakoydVh1U014cHRqcXpoWVE#gid=0'
    })
      .done(function(result) {
        // structure of result is below
        console.log(result);
      });

### Results

The result of fetch has a convenient structure of the following form:

    result = {
      records: // array of Objects
      fields: // array of Field Objects as per http://www.dataprotocols.org/en/latest/json-table-schema.html
      metadata: {
        spreadsheetTitle: ...,
        worksheetTitle: ...,
        title: spreadsheetTitle +" :: "+ result.worksheetTitle
      }
    }

You can also use GDocs parsing without depending on jQuery:

    // json should be the JSON you get from the Google Docs JSON API
    var out = recline.Backend.GDocs.parseData(json);

### Config options

You may specify info about the Google Spreadsheet in several ways

    // GDocs spreadsheet URL
    {
      url: 'https://docs.google.com/spreadsheet/ccc?key=0Aon3JiuouxLUdGlQVDJnbjZRSU1tUUJWOUZXRG53VkE#gid=0'
    }

    // OR the key 
    {
      url: '0Aon3JiuouxLUdDQwZE1JdV94cUd6NWtuZ0IyWTBjLWc'
    }

    // OR URL to API
    {
      url: 'https://spreadsheets.google.com/feeds/list/0Aon3JiuouxLUdDQwZE1JdV94cUd6NWtuZ0IyWTBjLWc/od6/public/values?alt=json'
    }

In addition you can provide a worksheet index (starting at 1):

    {
      url: ...
      worksheetIndex: 2
    }
 
NB: we try to guess from #gid={worksheetIndex} and o/w default to 1.  A
problem with guessing from gid is that the API worksheet indexes follow
the order of the worksheets as shown in the spreadsheet but #gid seems to
follow creation order so the gid and worksheetIndex may not be the same if
you have re-ordered spreadsheets


## Dependencies

* underscore
* jQuery (optional) - only if you want ajax requests
* underscore.deferred (optional) - only needed if no jQuery

One of the reasons for the different options is that it ensures you can use
this library in the browser *and* in webworkers (where jQuery does not
function).

