# `python-xpack`

A binary xPack with a standalone Python distribution

# Installation

```shell
npm install xpm
# with Python 3.12
npx xpm install @mmomtchev/python-xpack@3-12
# with Python 3.10
npx xpm install @mmomtchev/python-xpack@3-10
```

Then, as usual, everything launched from `xpm` scripts will automatically use this Python.

If you get SSL certificate validation problems on macOS, try pointing OpenSSL to the correct system certificate:

```shell
export SSL_CERT_FILE=/etc/ssl/cert.pem
```
