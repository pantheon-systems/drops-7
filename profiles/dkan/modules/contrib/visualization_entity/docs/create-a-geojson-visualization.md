## Setup

Make sure the geojson bundle is enabled:

```bash
$ drush -y visualization_entity_geojson_bundle
$ drush cc all
```

## Create Visualization

+ Look for **Content -> Add Content -> Resource** in the admin menu and click on it.

![](images/geojson-step-00.png)

+ Upload a **geojson** file for the resource

![](images/geojson-step-01.png)

+ Fill the required fields and **save** the resource

![](images/geojson-step-02.png)

+ Look for **Structure -> Entity Types -> Visualization -> Geojson Visualization -> Add Geojson Visualization** in the admin menu and click on it.

![](images/geojson-step-03.png)

+ Set a **title**
+ Select the **resource** containing the **geojson** file you uploaded

![](images/geojson-step-04.png)

+ Click **Save** & Enjoy!

![](images/geojson-step-05.png)
