# ANNIEDirt - Docker branch

## About
ANNIEDirt (g4annie_dirt_flux, g4dirt, g4annie, etc.) is part of ANNIE's simulation chain. It propigates final state particles from [GENIE](https://github.com/ANNIEsoft/GENIE-v3) until they are inside ANNIE's tank volume (or anyother specified volumes). This can drastically reduce run time when studing neutrino events outside the ANNIE tank.

## `/home/g4annie_dirt_flux` Usage
```
g4annie_dirt_flux -h | --help           print usage statment
                        -b | --batch          run in batch mode
                        -v | --verbose        increase verbosity
                        -n | --nevents        limit # of events
                        -r | --repeat         repeat each GENIE event in G4
                        -g | --gdml=GFILE     input geometry file
                        -p | --physics=PNAME  PhysicsList name
                        -i | --input=GNTP     GENIE file
                        -o | --output=OFILE   output file
```

## `/home/run_g4dirt.sh` Usage
```
run_g4dirt.sh -r=<run number (or numbers using `*`. Ex: \`-r='4*'\`)>
                    -i=</path/to/input/GENIE/files/dir>
                    -n=<number of events per GENIE file to propigate>
                    -g=</path/to/geometry/file.gdml>
                    -o=</path/to/output/dir>
                    -h|--help
```