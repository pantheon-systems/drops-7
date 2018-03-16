# Theming Guide

## Quick Start
1. Download and enable radix: 
`drush en radix -y; drush vset theme_default radix`

2. Create a subtheme: 
`drush cc all; drush radix "Subtheme"`

3. Set default theme:
`drush en subtheme -y; drush vset theme_default subtheme`

4. Install required modules
`cd /path/to/subtheme; npm run setup`

5. Update browserSyncProxy in /path/to/subtheme/config.json

6. Watch for changes:
`gulp`

