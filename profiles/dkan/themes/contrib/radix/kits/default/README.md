# Installation

{{Name}} theme uses [Gulp](http://gulpjs.com) to compile Sass. Gulp needs Node.

#### Step 1
Make sure you have Node and npm installed. 
You can read a guide on how to install node here: https://docs.npmjs.com/getting-started/installing-node

If you prefer to use [Yarn](https://yarnpkg.com) instead of npm, install Yarn by following the guide here: https://yarnpkg.com/docs/install. 

#### Step 2
Install bower: `npm install -g bower`.

#### Step 3
Go to the root of {{Name}} theme and run the following commands: `npm run setup`.

To install using Yarn, run `yarn install && bower install`.

#### Step 4
Update `browserSyncProxy` in **config.json**.

#### Step 5
Run the following command to compile Sass and watch for changes: `gulp`.
