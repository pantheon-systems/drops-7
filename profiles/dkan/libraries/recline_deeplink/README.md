### Recline Deeplink

Saves the current multiview state allowing to share a visualization by url.

## Requirements
* Recline multiview

**To save time we recommend to install this tools:**

* npm
* grunt
* bower

## Usage
You only have to pass a valid multiview object as value to recline.DeepLink.Router constructor.

```javascript
var router = new recline.DeepLink.Router(multiview);
router.start();
```

## Installation

```bash
git clone https://github.com/NuCivic/recline-deeplink.git
cd recline-deeplink
bower install
npm install
```

## Run demo

```bash
grunt
```

## Lint code

```bash
grunt lint
```

## Build example

```bash
make
```

## Plugins
A plugin is a javascript constructor that currently only need two methods to manipulate the url state and
react based on that state. You can add a new plugin just calling the addDependency method of router object in this way:

```javascript
router.addDependency(new recline.DeepLink.Deps.Map(map, router));
```

To define a new plugin you have to create a javascript constructor with the update and alterState methods and a property name set to the name that you want.

The alterState method allow you to add new data in the url under a key. The key used for that purpose is the name of the plugin.

Also you have to implement the update method that will be called when the name key is detected in the url.

You can check map.dep.js plugin implementation at src directory for more details.

## TODO
* Create unit tests

## Caveats
Since the state is shared through url, data edition (eg. add, delete or edit a row in the dataset) is not saved at all.
