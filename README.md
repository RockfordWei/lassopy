# Python 3.6 For Lasso 9

This is an experimental project for integrating Python 3.6 into Lasso 9

## Quick Test

```
$ git clone https://github.com/RockfordWei/lassopy.git && cd lassopy
$ docker pull rockywei/lasso:9
$ docker run -it -v $PWD:/home -w /home rockywei/lasso:9 /bin/bash -c "/home/test.sh"
```