ROOT=$PWD

cd $ROOT
cd ../../platform/macupdater

ant -Djedit.install.dir=$ROOT/build/jedit/jedit-program/ \
    -Dinstall.dir=$ROOT/build/macupdater
