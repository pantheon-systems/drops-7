# Nuboot Radix theme

This is the default theme for DKAN 1.0 -> https://github.com/NuCivic/dkan

## Using Drush
To start a new Nuboot Radix subtheme, run this command

```drush radix "MyThemeName" --kit=https://github.com/NuCivic/radix-kit-nuboot/archive/master.zip```

The new subtheme will be placed in to the /sites/all/themes/ directory, it will contain a few  files to get you started.


To upgrade an older NuBoot Radix subtheme to use the new Radix 7.x-3.3 compiling process, it will need to be done manually, follow the steps outlined here:
http://www.radixtheme.org/articles/update-rc4-to-3/

## Installation and Use

Your theme will use [Gulp](http://gulpjs.com) to compile Sass. Gulp needs Node.

#### Step 1
Make sure you have Node and npm installed.
You can read a guide on how to install node here: https://docs.npmjs.com/getting-started/installing-node

#### Step 2
Install bower: `npm install -g bower`.

#### Step 3
Go to the root of your theme and run the following commands: `npm run setup`.

#### Step 4
Update `browserSyncProxy` in **config.json**.

#### Step 5
Edit the files under the **scss** and **js** directory, these will be compiled into the **assets** directory.
Run the following command to compile Sass and watch for changes: `gulp`.

## Creating a new subtheme

When you need more control over the theme or prefer to keep customizations in code without using overrides, creating a subtheme of NuBoot Radix is an easy win. The subtheme will pick up all styles from the chain in cascading order: Radix > NuBoot Radix > Your Subtheme

So you only need to add styles where you want to override the upstream css, and add custom templates if you need specific changes to the layout.

To start a new Nuboot Radix subtheme, run this command

```
drush radix "MyThemeName" --kit=https://github.com/NuCivic/radix-kit-nuboot/archive/master.zip
```

The new subtheme will be placed in to the /sites/all/themes/ directory, it will appear to have a lot of files but it is mostly just file structure to get you started.

## Contributing

We are accepting issues in the dkan issue thread only -> https://github.com/NuCivic/dkan/issues -> Please label your issue as **"component: nuboot_radix"** after submitting so we can identify problems and feature requests faster.

If you can, please cross reference commits in this repo to the corresponding issue in the dkan issue thread. You can do that easily adding this text:

```
NuCivic/dkan#issue_id
```

to any commit message or comment replacing **issue_id** with the corresponding issue id.



## Credits for icon assets used in this project

**Streamline Icons** http://www.streamlineicons.com/index.html

**Flaticons** designed by Freepik http://www.flaticon.com/packs/file-formats-icons
