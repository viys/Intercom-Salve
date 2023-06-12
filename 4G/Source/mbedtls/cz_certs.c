#include "kingtls.h"

/* VeriSign CA certificate */
#define VERISIGN_CA_CRT \
"Verisign Class 3 Public Primary Certification Authority - G3\r\n"                     \
"============================================================\r\n"                     \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIEGjCCAwICEQCbfgZJoz5iudXukEhxKe9XMA0GCSqGSIb3DQEBBQUAMIHKMQswCQYDVQQGEwJV\r\n"     \
"UzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xHzAdBgNVBAsTFlZlcmlTaWduIFRydXN0IE5ldHdv\r\n"     \
"cmsxOjA4BgNVBAsTMShjKSAxOTk5IFZlcmlTaWduLCBJbmMuIC0gRm9yIGF1dGhvcml6ZWQgdXNl\r\n"     \
"IG9ubHkxRTBDBgNVBAMTPFZlcmlTaWduIENsYXNzIDMgUHVibGljIFByaW1hcnkgQ2VydGlmaWNh\r\n"     \
"dGlvbiBBdXRob3JpdHkgLSBHMzAeFw05OTEwMDEwMDAwMDBaFw0zNjA3MTYyMzU5NTlaMIHKMQsw\r\n"     \
"CQYDVQQGEwJVUzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xHzAdBgNVBAsTFlZlcmlTaWduIFRy\r\n"     \
"dXN0IE5ldHdvcmsxOjA4BgNVBAsTMShjKSAxOTk5IFZlcmlTaWduLCBJbmMuIC0gRm9yIGF1dGhv\r\n"     \
"cml6ZWQgdXNlIG9ubHkxRTBDBgNVBAMTPFZlcmlTaWduIENsYXNzIDMgUHVibGljIFByaW1hcnkg\r\n"     \
"Q2VydGlmaWNhdGlvbiBBdXRob3JpdHkgLSBHMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\r\n"     \
"ggEBAMu6nFL8eB8aHm8bN3O9+MlrlBIwT/A2R/XQkQr1F8ilYcEWQE37imGQ5XYgwREGfassbqb1\r\n"     \
"EUGO+i2tKmFZpGcmTNDovFJbcCAEWNF6yaRpvIMXZK0Fi7zQWM6NjPXr8EJJC52XJ2cybuGukxUc\r\n"     \
"cLwgTS8Y3pKI6GyFVxEa6X7jJhFUokWWVYPKMIno3Nij7SqAP395ZVc+FSBmCC+Vk7+qRy+oRpfw\r\n"     \
"EuL+wgorUeZ25rdGt+INpsyow0xZVYnm6FNcHOqd8GIWC6fJXwzw3sJ2zq/3avL6QaaiMxTJ5Xpj\r\n"     \
"055iN9WFZZ4O5lMkdBteHRJTW8cs54NJOxWuimi5V5cCAwEAATANBgkqhkiG9w0BAQUFAAOCAQEA\r\n"     \
"ERSWwauSCPc/L8my/uRan2Te2yFPhpk0djZX3dAVL8WtfxUfN2JzPtTnX84XA9s1+ivbrmAJXx5f\r\n"     \
"j267Cz3qWhMeDGBvtcC1IyIuBwvLqXTLR7sdwdela8wv0kL9Sd2nic9TutoAWii/gt/4uhMdUIaC\r\n"     \
"/Y4wjylGsB49Ndo4YhYYSq3mtlFs3q9i6wHQHiT+eo8SGhJouPtmmRQURVyu565pF4ErWjfJXir0\r\n"     \
"xuKhXFSbplQAz/DxwceYMBo7Nhbbo27q/a2ywtrvAkcTisDxszGtTxzhT5yvDwyd93gN2PQ1VoDa\r\n"     \
"t20Xj50egWTh/sVFuq1ruQp6Tk9LhO5L8X3dEQ==\r\n"                                         \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"Verisign Class 4 Public Primary Certification Authority - G3\r\n"                     \
"============================================================\r\n"                     \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIEGjCCAwICEQDsoKeLbnVqAc/EfMwvlF7XMA0GCSqGSIb3DQEBBQUAMIHKMQswCQYDVQQGEwJV\r\n"     \
"UzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xHzAdBgNVBAsTFlZlcmlTaWduIFRydXN0IE5ldHdv\r\n"     \
"cmsxOjA4BgNVBAsTMShjKSAxOTk5IFZlcmlTaWduLCBJbmMuIC0gRm9yIGF1dGhvcml6ZWQgdXNl\r\n"     \
"IG9ubHkxRTBDBgNVBAMTPFZlcmlTaWduIENsYXNzIDQgUHVibGljIFByaW1hcnkgQ2VydGlmaWNh\r\n"     \
"dGlvbiBBdXRob3JpdHkgLSBHMzAeFw05OTEwMDEwMDAwMDBaFw0zNjA3MTYyMzU5NTlaMIHKMQsw\r\n"     \
"CQYDVQQGEwJVUzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xHzAdBgNVBAsTFlZlcmlTaWduIFRy\r\n"     \
"dXN0IE5ldHdvcmsxOjA4BgNVBAsTMShjKSAxOTk5IFZlcmlTaWduLCBJbmMuIC0gRm9yIGF1dGhv\r\n"     \
"cml6ZWQgdXNlIG9ubHkxRTBDBgNVBAMTPFZlcmlTaWduIENsYXNzIDQgUHVibGljIFByaW1hcnkg\r\n"     \
"Q2VydGlmaWNhdGlvbiBBdXRob3JpdHkgLSBHMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoC\r\n"     \
"ggEBAK3LpRFpxlmr8Y+1GQ9Wzsy1HyDkniYlS+BzZYlZ3tCD5PUPtbut8XzoIfzk6AzufEUiGXaS\r\n"     \
"tBO3IFsJ+mGuqPKljYXCKtbeZjbSmwL0qJJgfJxptI8kHtCGUvYynEFYHiK9zUVilQhu0GbdU6LM\r\n"     \
"8BDcVHOLBKFGMzNcF0C5nk3T875Vg+ixiY5afJqWIpA7iCXy0lOIAgwLePLmNxdLMEYH5IBtptiW\r\n"     \
"Lugs+BGzOA1mppvqySNb247i8xOOGlktqgLw7KSHZtzBP/XYufTsgsbSPZUd5cBPhMnZo0QoBmrX\r\n"     \
"Razwa2rvTl/4EYIeOGM0ZlDUPpNz+jDDZq3/ky2X7wMCAwEAATANBgkqhkiG9w0BAQUFAAOCAQEA\r\n"     \
"j/ola09b5KROJ1WrIhVZPMq1CtRK26vdoV9TxaBXOcLORyu+OshWv8LZJxA6sQU8wHcxuzrTBXtt\r\n"     \
"mhwwjIDLk5Mqg6sFUYICABFna/OIYUdfA5PVWw3g8dShMjWFsjrbsIKr0csKvE+MW8VLADsfKoKm\r\n"     \
"fjaF3H48ZwC15DtS4KjrXRX5xm3wrR0OhbepmnMUWluPQSjA1egtTaRezarZ7c7c2NU8Qh0XwRJd\r\n"     \
"RTjDOPP8hS6DRkiy1yBfkjaP53kPmF6Z6PDQpLv1U70qzlmwr25/bLvSHgCwIe34QWKCudiyxLtG\r\n"     \
"UPMxxY8BqHTr9Xgn2uf3ZkPznoM+IKrDNWCRzg==\r\n"                                         \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"VeriSign Class 3 Public Primary Certification Authority - G5\r\n"                     \
"============================================================\r\n"                     \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCByjELMAkGA1UE\r\n"     \
"BhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2lnbiBUcnVzdCBO\r\n"     \
"ZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVk\r\n"     \
"IHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRp\r\n"     \
"ZmljYXRpb24gQXV0aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCB\r\n"     \
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2ln\r\n"     \
"biBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2lnbiwgSW5jLiAtIEZvciBh\r\n"     \
"dXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmlt\r\n"     \
"YXJ5IENlcnRpZmljYXRpb24gQXV0aG9yaXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAw\r\n"     \
"ggEKAoIBAQCvJAgIKXo1nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKz\r\n"     \
"j/i5Vbext0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIzSdhD\r\n"     \
"Y2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQGBO+QueQA5N06tRn/\r\n"     \
"Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+rCpSx4/VBEnkjWNHiDxpg8v+R70r\r\n"     \
"fk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/\r\n"     \
"BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2Uv\r\n"     \
"Z2lmMCEwHzAHBgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\r\n"     \
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKvMzEzMA0GCSqG\r\n"     \
"SIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzEp6B4Eq1iDkVwZMXnl2YtmAl+\r\n"     \
"X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKE\r\n"     \
"KQsTb47bDN0lAtukixlE0kF6BWlKWE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiC\r\n"     \
"Km0oHw0LxOXnGiYZ4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vE\r\n"     \
"ZV8NhnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\r\n"                         \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"VeriSign Universal Root Certification Authority\r\n"                                  \
"===============================================\r\n"                                  \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIEuTCCA6GgAwIBAgIQQBrEZCGzEyEDDrvkEhrFHTANBgkqhkiG9w0BAQsFADCBvTELMAkGA1UE\r\n"     \
"BhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2lnbiBUcnVzdCBO\r\n"     \
"ZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwOCBWZXJpU2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVk\r\n"     \
"IHVzZSBvbmx5MTgwNgYDVQQDEy9WZXJpU2lnbiBVbml2ZXJzYWwgUm9vdCBDZXJ0aWZpY2F0aW9u\r\n"     \
"IEF1dGhvcml0eTAeFw0wODA0MDIwMDAwMDBaFw0zNzEyMDEyMzU5NTlaMIG9MQswCQYDVQQGEwJV\r\n"     \
"UzEXMBUGA1UEChMOVmVyaVNpZ24sIEluYy4xHzAdBgNVBAsTFlZlcmlTaWduIFRydXN0IE5ldHdv\r\n"     \
"cmsxOjA4BgNVBAsTMShjKSAyMDA4IFZlcmlTaWduLCBJbmMuIC0gRm9yIGF1dGhvcml6ZWQgdXNl\r\n"     \
"IG9ubHkxODA2BgNVBAMTL1ZlcmlTaWduIFVuaXZlcnNhbCBSb290IENlcnRpZmljYXRpb24gQXV0\r\n"     \
"aG9yaXR5MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAx2E3XrEBNNti1xWb/1hajCMj\r\n"     \
"1mCOkdeQmIN65lgZOIzF9uVkhbSicfvtvbnazU0AtMgtc6XHaXGVHzk8skQHnOgO+k1KxCHfKWGP\r\n"     \
"MiJhgsWHH26MfF8WIFFE0XBPV+rjHOPMee5Y2A7Cs0WTwCznmhcrewA3ekEzeOEz4vMQGn+HLL72\r\n"     \
"9fdC4uW/h2KJXwBL38Xd5HVEMkE6HnFuacsLdUYI0crSK5XQz/u5QGtkjFdN/BMReYTtXlT2NJ8I\r\n"     \
"AfMQJQYXStrxHXpma5hgZqTZ79IugvHw7wnqRMkVauIDbjPTrJ9VAMf2CGqUuV/c4DPxhGD5WycR\r\n"     \
"tPwW8rtWaoAljQIDAQABo4GyMIGvMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgEGMG0G\r\n"     \
"CCsGAQUFBwEMBGEwX6FdoFswWTBXMFUWCWltYWdlL2dpZjAhMB8wBwYFKw4DAhoEFI/l0xqGrI2O\r\n"     \
"a8PPgGrUSBgsexkuMCUWI2h0dHA6Ly9sb2dvLnZlcmlzaWduLmNvbS92c2xvZ28uZ2lmMB0GA1Ud\r\n"     \
"DgQWBBS2d/ppSEefUxLVwuoHMnYH0ZcHGTANBgkqhkiG9w0BAQsFAAOCAQEASvj4sAPmLGd75JR3\r\n"     \
"Y8xuTPl9Dg3cyLk1uXBPY/ok+myDjEedO2Pzmvl2MpWRsXe8rJq+seQxIcaBlVZaDrHC1LGmWazx\r\n"     \
"Y8u4TB1ZkErvkBYoH1quEPuBUDgMbMzxPcP1Y+Oz4yHJJDnp/RVmRvQbEdBNc6N9Rvk97ahfYtTx\r\n"     \
"P/jgdFcrGJ2BtMQo2pSXpXDrrB2+BxHw1dvd5Yzw1TKwg+ZX4o+/vqGqvz0dtdQ46tewXDpPaj+P\r\n"     \
"wGZsY6rp2aQW9IHRlRQOfc2VNNnSj3BzgXucfr2YYdhFh5iQxeuGMMY1v/D/w1WIg0vvBZIGcfK4\r\n"     \
"mJO37M2CYfE45k+XmCpajQ==\r\n"                                                         \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"VeriSign Class 3 Public Primary Certification Authority - G4\r\n"                     \
"============================================================\r\n"                     \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIDhDCCAwqgAwIBAgIQL4D+I4wOIg9IZxIokYesszAKBggqhkjOPQQDAzCByjELMAkGA1UEBhMC\r\n"     \
"VVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2lnbiBUcnVzdCBOZXR3\r\n"     \
"b3JrMTowOAYDVQQLEzEoYykgMjAwNyBWZXJpU2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVz\r\n"     \
"ZSBvbmx5MUUwQwYDVQQDEzxWZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmlj\r\n"     \
"YXRpb24gQXV0aG9yaXR5IC0gRzQwHhcNMDcxMTA1MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCByjEL\r\n"     \
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZWZXJpU2lnbiBU\r\n"     \
"cnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNyBWZXJpU2lnbiwgSW5jLiAtIEZvciBhdXRo\r\n"     \
"b3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5\r\n"     \
"IENlcnRpZmljYXRpb24gQXV0aG9yaXR5IC0gRzQwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAASnVnp8\r\n"     \
"Utpkmw4tXNherJI9/gHmGUo9FANL+mAnINmDiWn6VMaaGF5VKmTeBvaNSjutEDxlPZCIBIngMGGz\r\n"     \
"rl0Bp3vefLK+ymVhAIau2o970ImtTR1ZmkGxvEeA3J5iw/mjgbIwga8wDwYDVR0TAQH/BAUwAwEB\r\n"     \
"/zAOBgNVHQ8BAf8EBAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEw\r\n"     \
"HzAHBgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVyaXNpZ24u\r\n"     \
"Y29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFLMWkf3upm7ktS5Jj4d4gYDs5bG1MAoGCCqGSM49BAMD\r\n"     \
"A2gAMGUCMGYhDBgmYFo4e1ZC4Kf8NoRRkSAsdk1DPcQdhCPQrNZ8NQbOzWm9kA3bbEhCHQ6qQgIx\r\n"     \
"AJw9SDkjOVgaFRJZap7v1VmyHVIsmXHNxynfGyphe3HR3vPA5Q06Sqotp9iGKt0uEA==\r\n"             \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"VeriSign Class 3 Secure Server CA - G3\r\n"                                           \
"======================================\r\n"                                           \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIF7DCCBNSgAwIBAgIQbsx6pacDIAm4zrz06VLUkTANBgkqhkiG9w0BAQUFADCB\r\n"                 \
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\r\n"                 \
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\r\n"                 \
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\r\n"                 \
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\r\n"                 \
"aG9yaXR5IC0gRzUwHhcNMTAwMjA4MDAwMDAwWhcNMjAwMjA3MjM1OTU5WjCBtTEL\r\n"                 \
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\r\n"                 \
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTswOQYDVQQLEzJUZXJtcyBvZiB1c2UgYXQg\r\n"                 \
"aHR0cHM6Ly93d3cudmVyaXNpZ24uY29tL3JwYSAoYykxMDEvMC0GA1UEAxMmVmVy\r\n"                 \
"aVNpZ24gQ2xhc3MgMyBTZWN1cmUgU2VydmVyIENBIC0gRzMwggEiMA0GCSqGSIb3\r\n"                 \
"DQEBAQUAA4IBDwAwggEKAoIBAQCxh4QfwgxF9byrJZenraI+nLr2wTm4i8rCrFbG\r\n"                 \
"5btljkRPTc5v7QlK1K9OEJxoiy6Ve4mbE8riNDTB81vzSXtig0iBdNGIeGwCU/m8\r\n"                 \
"f0MmV1gzgzszChew0E6RJK2GfWQS3HRKNKEdCuqWHQsV/KNLO85jiND4LQyUhhDK\r\n"                 \
"tpo9yus3nABINYYpUHjoRWPNGUFP9ZXse5jUxHGzUL4os4+guVOc9cosI6n9FAbo\r\n"                 \
"GLSa6Dxugf3kzTU2s1HTaewSulZub5tXxYsU5w7HnO1KVGrJTcW/EbGuHGeBy0RV\r\n"                 \
"M5l/JJs/U0V/hhrzPPptf4H1uErT9YU3HLWm0AnkGHs4TvoPAgMBAAGjggHfMIIB\r\n"                 \
"2zA0BggrBgEFBQcBAQQoMCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLnZlcmlz\r\n"                 \
"aWduLmNvbTASBgNVHRMBAf8ECDAGAQH/AgEAMHAGA1UdIARpMGcwZQYLYIZIAYb4\r\n"                 \
"RQEHFwMwVjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cudmVyaXNpZ24uY29tL2Nw\r\n"                 \
"czAqBggrBgEFBQcCAjAeGhxodHRwczovL3d3dy52ZXJpc2lnbi5jb20vcnBhMDQG\r\n"                 \
"A1UdHwQtMCswKaAnoCWGI2h0dHA6Ly9jcmwudmVyaXNpZ24uY29tL3BjYTMtZzUu\r\n"                 \
"Y3JsMA4GA1UdDwEB/wQEAwIBBjBtBggrBgEFBQcBDARhMF+hXaBbMFkwVzBVFglp\r\n"                 \
"bWFnZS9naWYwITAfMAcGBSsOAwIaBBSP5dMahqyNjmvDz4Bq1EgYLHsZLjAlFiNo\r\n"                 \
"dHRwOi8vbG9nby52ZXJpc2lnbi5jb20vdnNsb2dvLmdpZjAoBgNVHREEITAfpB0w\r\n"                 \
"GzEZMBcGA1UEAxMQVmVyaVNpZ25NUEtJLTItNjAdBgNVHQ4EFgQUDURcFlNEwYJ+\r\n"                 \
"HSCrJfQBY9i+eaUwHwYDVR0jBBgwFoAUf9Nlp8Ld7LvwMAnzQzn6Aq8zMTMwDQYJ\r\n"                 \
"KoZIhvcNAQEFBQADggEBAAyDJO/dwwzZWJz+NrbrioBL0aP3nfPMU++CnqOh5pfB\r\n"                 \
"WJ11bOAdG0z60cEtBcDqbrIicFXZIDNAMwfCZYP6j0M3m+oOmmxw7vacgDvZN/R6\r\n"                 \
"bezQGH1JSsqZxxkoor7YdyT3hSaGbYcFQEFn0Sc67dxIHSLNCwuLvPSxe/20majp\r\n"                 \
"dirhGi2HbnTTiN0eIsbfFrYrghQKlFzyUOyvzv9iNw2tZdMGQVPtAhTItVgooazg\r\n"                 \
"W+yzf5VK+wPIrSbb5mZ4EkrZn0L74ZjmQoObj49nJOhhGbXdzbULJgWOw27EyHW4\r\n"                 \
"Rs/iGAZeqa6ogZpHFt4MKGwlJ7net4RYxh84HqTEy2Y=\r\n"                                     \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"Verisign Class 3 Public Primary Certification Authority\r\n"                          \
"=======================================================\r\n"                          \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIICPDCCAaUCEHC65B0Q2Sk0tjjKewPMur8wDQYJKoZIhvcNAQECBQAwXzELMAkGA1UEBhMCVVMx\r\n"     \
"FzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMTcwNQYDVQQLEy5DbGFzcyAzIFB1YmxpYyBQcmltYXJ5\r\n"     \
"IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTk2MDEyOTAwMDAwMFoXDTI4MDgwMTIzNTk1OVow\r\n"     \
"XzELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMTcwNQYDVQQLEy5DbGFzcyAz\r\n"     \
"IFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9yaXR5MIGfMA0GCSqGSIb3DQEBAQUA\r\n"     \
"A4GNADCBiQKBgQDJXFme8huKARS0EN8EQNvjV69qRUCPhAwL0TPZ2RHP7gJYHyX3KqhEBarsAx94\r\n"     \
"f56TuZoAqiN91qyFomNFx3InzPRMxnVx0jnvT0Lwdd8KkMaOIG+YD/isI19wKTakyYbnsZogy1Ol\r\n"     \
"hec9vn2a/iRFM9x2Fe0PonFkTGUugWhFpwIDAQABMA0GCSqGSIb3DQEBAgUAA4GBALtMEivPLCYA\r\n"     \
"TxQT3ab7/AoRhIzzKBxnki98tsX63/Dolbwdj2wsqFHMc9ikwFPwTtYmwHYBV4GSXiHx0bH/59Ah\r\n"     \
"WM1pF+NEHJwZRDmJXNycAA9WjQKZ7aKQRUzkuxCkPfAyAw7xzvjoyVGM5mKf5p/AfbdynMk2Omuf\r\n"     \
"Tqj/ZA1k\r\n"                                                                         \
"-----END CERTIFICATE-----\r\n"                                                        \
"\r\n"                                                                                 \
"Verisign Class 3 Public Primary Certification Authority - G2\r\n"                     \
"============================================================\r\n"                     \
"-----BEGIN CERTIFICATE-----\r\n"                                                      \
"MIIDAjCCAmsCEH3Z/gfPqB63EHln+6eJNMYwDQYJKoZIhvcNAQEFBQAwgcExCzAJBgNVBAYTAlVT\r\n"     \
"MRcwFQYDVQQKEw5WZXJpU2lnbiwgSW5jLjE8MDoGA1UECxMzQ2xhc3MgMyBQdWJsaWMgUHJpbWFy\r\n"     \
"eSBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eSAtIEcyMTowOAYDVQQLEzEoYykgMTk5OCBWZXJpU2ln\r\n"     \
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MR8wHQYDVQQLExZWZXJpU2lnbiBUcnVz\r\n"     \
"dCBOZXR3b3JrMB4XDTk4MDUxODAwMDAwMFoXDTI4MDgwMTIzNTk1OVowgcExCzAJBgNVBAYTAlVT\r\n"     \
"MRcwFQYDVQQKEw5WZXJpU2lnbiwgSW5jLjE8MDoGA1UECxMzQ2xhc3MgMyBQdWJsaWMgUHJpbWFy\r\n"     \
"eSBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eSAtIEcyMTowOAYDVQQLEzEoYykgMTk5OCBWZXJpU2ln\r\n"     \
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MR8wHQYDVQQLExZWZXJpU2lnbiBUcnVz\r\n"     \
"dCBOZXR3b3JrMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDMXtERXVxp0KvTuWpMmR9ZmDCO\r\n"     \
"FoUgRm1HP9SFIIThbbP4pO0M8RcPO/mn+SXXwc+EY/J8Y8+iR/LGWzOOZEAEaMGAuWQcRXfH2G71\r\n"     \
"lSk8UOg013gfqLptQ5GVj0VXXn7F+8qkBOvqlzdUMG+7AUcyM83cV5tkaWH4mx0ciU9cZwIDAQAB\r\n"     \
"MA0GCSqGSIb3DQEBBQUAA4GBAFFNzb5cy5gZnBWyATl4Lk0PZ3BwmcYQWpSkU01UbSuvDV1Ai2TT\r\n"     \
"1+7eVmGSX6bEHRBhNtMsJzzoKQm5EWR0zLVznxxIqbxhAe7iF6YM40AIOw7n60RzKprxaZLvcRTD\r\n"     \
"Oaxxp5EJb+RxBrO6WVcmeQD2+A2iMzAo1KpYoJ2daZH9\r\n"                                     \
"-----END CERTIFICATE-----\r\n"
    
#define CHEERZING_TEST_CA_CRT \
"-----BEGIN CERTIFICATE-----\r\n"                                          \
"MIICVTCCAb4CCQCPmKvbfWPgazANBgkqhkiG9w0BAQUFADBvMQswCQYDVQQGEwJx\r\n"     \
"ajELMAkGA1UECAwCcWoxCzAJBgNVBAcMAnFqMQswCQYDVQQKDAJxajELMAkGA1UE\r\n"     \
"CwwCcWoxCzAJBgNVBAMMAnFqMR8wHQYJKoZIhvcNAQkBFhBub3Jlc3RhcnRAcXEu\r\n"     \
"Y29tMB4XDTE2MDQyOTA5Mjc0MFoXDTI2MDQyNzA5Mjc0MFowbzELMAkGA1UEBhMC\r\n"     \
"cWoxCzAJBgNVBAgMAnFqMQswCQYDVQQHDAJxajELMAkGA1UECgwCcWoxCzAJBgNV\r\n"     \
"BAsMAnFqMQswCQYDVQQDDAJxajEfMB0GCSqGSIb3DQEJARYQbm9yZXN0YXJ0QHFx\r\n"     \
"LmNvbTCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEA8Kk3hNYB28khcNRv08Rt\r\n"     \
"Y2v2PeAkOvBalppjvrp3SIrtJOi5vKvwcU4kD8XYQTYwrzCdbx97kfRlHFkC1fdL\r\n"     \
"MbbjA0baqKKdzUi0PCHCn1EvM4nYvd+psWmskYAqKlgxj4YwVnv98bifuCRgETgb\r\n"     \
"U+1ZX1gR743njUxrnvXqj20CAwEAATANBgkqhkiG9w0BAQUFAAOBgQCCC4AAjUx3\r\n"     \
"lgWKlTQQP9VCodoruDZ/PnUd1jVhQR5533dOmPUcxe5XwW7YxFnUoVk7WaJkLdGL\r\n"     \
"AoZ48Qm42Fj8DiRHzg7w2tlAc8HzPCLucm6JH58cBMMC9FDK24FJnIvzAht2w5Sj\r\n"     \
"lf5IWl035Y6lYSLNnBGhgNX/W2gsOC9VhA==\r\n"                                 \
"-----END CERTIFICATE-----\r\n"

#define CHEERZING_TEST_CLI_CRT_RSA \
"-----BEGIN CERTIFICATE-----\r\n"                                          \
"MIIDPzCCAiegAwIBAgIBBDANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\r\n"     \
"MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"     \
"MTEwMjEyMTQ0NDA3WhcNMjEwMjEyMTQ0NDA3WjA8MQswCQYDVQQGEwJOTDERMA8G\r\n"     \
"A1UEChMIUG9sYXJTU0wxGjAYBgNVBAMTEVBvbGFyU1NMIENsaWVudCAyMIIBIjAN\r\n"     \
"BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6f\r\n"     \
"M60Nj4o8VmXl3ETZzGaFB9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu\r\n"     \
"1C93KYRhTYJQj6eVSHD1bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEw\r\n"     \
"MjDV0/YI0FZPRo7yX/k9Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v\r\n"     \
"4Jv4EFbMs44TFeY0BGbH7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx/\r\n"     \
"/DZrtenNLQNiTrM9AM+vdqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQAB\r\n"     \
"o00wSzAJBgNVHRMEAjAAMB0GA1UdDgQWBBRxoQBzckAvVHZeM/xSj7zx3WtGITAf\r\n"     \
"BgNVHSMEGDAWgBS0WuSls97SUva51aaVD+s+vMf9/zANBgkqhkiG9w0BAQUFAAOC\r\n"     \
"AQEAAn86isAM8X+mVwJqeItt6E9slhEQbAofyk+diH1Lh8Y9iLlWQSKbw/UXYjx5\r\n"     \
"LLPZcniovxIcARC/BjyZR9g3UwTHNGNm+rwrqa15viuNOFBchykX/Orsk02EH7NR\r\n"     \
"Alw5WLPorYjED6cdVQgBl9ot93HdJogRiXCxErM7NC8/eP511mjq+uLDjLKH8ZPQ\r\n"     \
"8I4ekHJnroLsDkIwXKGIsvIBHQy2ac/NwHLCQOK6mfum1pRx52V4Utu5dLLjD5bM\r\n"     \
"xOBC7KU4xZKuMXXZM6/93Yb51K/J4ahf1TxJlTWXtnzDr9saEYdNy2SKY/6ZiDNH\r\n"     \
"D+stpAKiQLAWaAusIWKYEyw9MQ==\r\n"                                         \
"-----END CERTIFICATE-----\r\n"

#define CHEERZING_TEST_CLI_KEY_RSA \
"-----BEGIN RSA PRIVATE KEY-----\r\n"                                          \
"MIIEpAIBAAKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6fM60Nj4o8VmXl3ETZzGaF\r\n"     \
"B9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu1C93KYRhTYJQj6eVSHD1\r\n"     \
"bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEwMjDV0/YI0FZPRo7yX/k9\r\n"     \
"Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v4Jv4EFbMs44TFeY0BGbH\r\n"     \
"7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx//DZrtenNLQNiTrM9AM+v\r\n"     \
"dqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQABAoIBAGdNtfYDiap6bzst\r\n"     \
"yhCiI8m9TtrhZw4MisaEaN/ll3XSjaOG2dvV6xMZCMV+5TeXDHOAZnY18Yi18vzz\r\n"     \
"4Ut2TnNFzizCECYNaA2fST3WgInnxUkV3YXAyP6CNxJaCmv2aA0yFr2kFVSeaKGt\r\n"     \
"ymvljNp2NVkvm7Th8fBQBO7I7AXhz43k0mR7XmPgewe8ApZOG3hstkOaMvbWAvWA\r\n"     \
"zCZupdDjZYjOJqlA4eEA4H8/w7F83r5CugeBE8LgEREjLPiyejrU5H1fubEY+h0d\r\n"     \
"l5HZBJ68ybTXfQ5U9o/QKA3dd0toBEhhdRUDGzWtjvwkEQfqF1reGWj/tod/gCpf\r\n"     \
"DFi6X0ECgYEA4wOv/pjSC3ty6TuOvKX2rOUiBrLXXv2JSxZnMoMiWI5ipLQt+RYT\r\n"     \
"VPafL/m7Dn6MbwjayOkcZhBwk5CNz5A6Q4lJ64Mq/lqHznRCQQ2Mc1G8eyDF/fYL\r\n"     \
"Ze2pLvwP9VD5jTc2miDfw+MnvJhywRRLcemDFP8k4hQVtm8PMp3ZmNECgYEA4gz7\r\n"     \
"wzObR4gn8ibe617uQPZjWzUj9dUHYd+in1gwBCIrtNnaRn9I9U/Q6tegRYpii4ys\r\n"     \
"c176NmU+umy6XmuSKV5qD9bSpZWG2nLFnslrN15Lm3fhZxoeMNhBaEDTnLT26yoi\r\n"     \
"33gp0mSSWy94ZEqipms+ULF6sY1ZtFW6tpGFoy8CgYAQHhnnvJflIs2ky4q10B60\r\n"     \
"ZcxFp3rtDpkp0JxhFLhiizFrujMtZSjYNm5U7KkgPVHhLELEUvCmOnKTt4ap/vZ0\r\n"     \
"BxJNe1GZH3pW6SAvGDQpl9sG7uu/vTFP+lCxukmzxB0DrrDcvorEkKMom7ZCCRvW\r\n"     \
"KZsZ6YeH2Z81BauRj218kQKBgQCUV/DgKP2985xDTT79N08jUo3hTP5MVYCCuj/+\r\n"     \
"UeEw1TvZcx3LJby7P6Xad6a1/BqveaGyFKIfEFIaBUBItk801sDDpDaYc4gL00Xc\r\n"     \
"7lFuBHOZkxJYlss5QrGpuOEl9ZwUt5IrFLBdYaKqNHzNVC1pCPfb/JyH6Dr2HUxq\r\n"     \
"gxUwAQKBgQCcU6G2L8AG9d9c0UpOyL1tMvFe5Ttw0KjlQVdsh1MP6yigYo9DYuwu\r\n"     \
"bHFVW2r0dBTqegP2/KTOxKzaHfC1qf0RGDsUoJCNJrd1cwoCLG8P2EF4w3OBrKqv\r\n"     \
"8u4ytY0F+Vlanj5lm3TaoHSVF1+NWPyOTiwevIECGKwSxvlki4fDAA==\r\n"             \
"-----END RSA PRIVATE KEY-----\r\n"

const char mbedtls_cas_pem[] = VERISIGN_CA_CRT;
const size_t mbedtls_cas_pem_len = sizeof( mbedtls_cas_pem );

const char mbedtls_cheerzing_test_pem[] = CHEERZING_TEST_CA_CRT;
const size_t mbedtls_cheerzing_test_pem_len = sizeof( mbedtls_cheerzing_test_pem );

const char mbedtls_cheerzing_test_cli_crt[] = CHEERZING_TEST_CLI_CRT_RSA;
const char mbedtls_cheerzing_test_cli_key[] = CHEERZING_TEST_CLI_KEY_RSA;
const size_t mbedtls_cheerzing_test_cli_crt_len = sizeof( mbedtls_cheerzing_test_cli_crt );
const size_t mbedtls_cheerzing_test_cli_key_len = sizeof( mbedtls_cheerzing_test_cli_key );
