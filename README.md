## About SquinkyVCV
This project is a frequency shifter module for VCV Rack. Over time project is expected to grow from a single module to a collection of several modules.

You can find us on Facebook [here](https://www.facebook.com/SquinkyLabs)
## Manuals
Here is the user’s manual for the [frequency shifter](./docs/booty-shifter.md).
## Contributing
Please use our GitHub issues page to report problems, request features, etc. If you don’t already have a GitHub account you will need to create one, as you must be logged in to post to GitHub.

For general communications, you may use our [Facebook Page](https://www.facebook.com/SquinkyLabs)

We are not currently accepting pull requests.
## Installing binaries
Download the current release from our [releases page](https://github.com/squinkylabs/SquinkyVCV/releases).
Follow the standard instructions for installing third-party plugins in rack: [here](https://vcvrack.com/manual/Installing.html)

Note that the built-in unzip in windows explorer will create an extra folder called with the same name as the zip file. But when everything is unzipped you want your folder structure to look like this:
```
plugins/
    <other plugins>
    squinkylabs-plug1/
        plugin.dll
        plugin.dylib
        plugin.so
        LICENSE
        res/   
            <one or more svg files>
            <possibly other resources
```
    
## Building source
As with all third-party modules for VCV, you must:
* Clone the VCV Rack repo.
* Build Rack from source.
* Clone SquinkyVCV in Rack’s plugins folder.
* `CD SquinkyVCV`
* `make`

## VCV Rack version 0.6.0
The next version of VCV Rack is coming soon. Version 0.5.x plugins will not work with version 0.6.0.

We are almost done porting our work to 0.6.0. That work is currently on a branch called v0.6.0dev. If you already have a build of VCV Rack 0.6.0 then you may build our plugin by running `git checkout v0.6.0dev` before running `make`.
