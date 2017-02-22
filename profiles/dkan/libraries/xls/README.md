Install
-------------------------------------------------------------------------------
Just include recline.backend.xlsx.min.js in your html. See dependencies bellow.

```javascript
<script type"text/javascript" src="dist/recline.backend.xlsx.min.js"></script>
```

Usage
-------------------------------------------------------------------------------
```javascript
;(function($) {
  'use strict';

  $(document).on('ready', function(){

    var backend = {
      backend: 'xlsx',
      url: 'data/example.xlsx',
      sheet: 'apollo-parsed-1737-325_0'
    };

    Excel.fetch(backend).done(function(data){
      console.log(data);
    });

  });
})(jQuery);
```

Dependencies
-------------------------------------------------------------------------------
* underscore or lodash
* SheetJS
* underscore.deferred (optional) - only needed if no jQuery

## Examples

Requirements
-------------------------------------------------------------------------------
* NodeJS
* NPM
* Bower
* Make

Getting started
-------------------------------------------------------------------------------

* Install Grunt globally

```bash
npm install -g grunt-cli
```

* Install node packages

```bash
npm install
```

* Install bower packages

```bash
bower install
```

* Run server

```bash
grunt
```

* Make a build

```bash
grunt build
```

* Lint code

```bash
grunt lint
```
