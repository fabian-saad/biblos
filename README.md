# Biblos

Biblos is a Bible reader for the terminal written in C. It allows you to search for Bible passages, compare different translations, and customize the displayed references.

## Features

* Search for Bible passages by book, chapter, and verse reference
* Support for multiple Bible translations in German and English (maybe more to come)
* Multiple translations can be selected at once
* Multiple book aliases for easier usage
* Verse ranges and multiple passage selections
* Option for extended references
* Option to hide the reference at the end
* Option to hide verse numbers in the text
* Show all available translations

## Usage

```text
Usage: ./biblos [-e] [-r] [-t translation]... [-v] passage...
       ./biblos -h
       ./biblos -T
```

The default translation is **Schlachter (German)**.

### Options

| Option           | Description                                           |
| ---------------- | ----------------------------------------------------- |
| `-h`             | Display the help message                              |
| `-e`             | Display the extended reference                        |
| `-r`             | Do not display the reference at the end               |
| `-t translation` | Select a translation (can be repeated multiple times) |
| `-T`             | Display all available translations                    |
| `-v`             | Do not display verse numbers in the text              |

### Supported Translations

```console
$ ./biblos -T
Die Schlachter-Bibel 1951 (SCH), German
Lutherbibel 1912 (L12), German
Darby Unrevidierte Elberfelder (ELO), German
Textbibel von Kautzsch und Weizsäcker (TKW), German
Elberfelder Übersetzung (Version von bibelkommentare.de) (ELBBK), German
King James (Authorized) Version (KJAV), English
American Standard Version (1901) (ASV), English
Darby Translation (DBY), English
Literal Standard Version (LSV), English
NET Bible (NETB), English
Young's Literal Translation (YLT), English
Bible in Basic English (BBE), English
Free Bible Version (FBV), English
Geneva Bible 1599 (GNV), English
Majority Standard Bible (MSB), English
The Orthodox Jewish Bible (TOJB), English
Translation for Translators (T4T), English
Unlocked Literal Bible (ULB), English
Noah Webster Bible (NWB), English
World Messianic Bible British Edition (WMBBE), English
World Messianic Bible (WMB), English
World English Bible British Edition (WEBBE), English
```

## Examples

A single verse:

```bash
./biblos isa8:31
```
The first verse:

```bash
./biblos genesis1:
```

Multiple verses in a chapter:

```bash
./biblos joh3:16,18,21
```

Range of verses:

```bash
./biblos jhn1:14-17
```

All verses to the end or from the start of the chapter:

```bash
./biblos dan7:20-
./biblos daniel8:-14
```

A whole chapter:

```bash
./biblos psa51
```

Multiple passages:

```bash
./biblos rom8:28,31,38-39 ps23 ephesians4:32
```

Multiple passages, same book:

```bash
./biblos revel21:5 22:3-5
```

Use other translations:

```bash
./biblos -t kjv luke16:10
```
Passages using multiple translations:

```bash
./biblos -t luth -t sch -t kjv eph4:32 matt6:33
```

Hide the final reference and the verse number in the text:

```bash
./biblos -r -v james1:19
```

Display an extended final reference:

```bash
./biblos -e matthew1
```

## Installation

Install the required dependencies:

```bash
sudo apt update
sudo apt install gcc make libcjson-dev
```

## Building

Build the program with:

```bash
make
```

To remove the compiled files:

```bash
make clean
```

## Data Sources

The Bible texts included with Biblos are provided by the [Free Use Bible API](https://bible.helloao.org/) by AO Lab.

The Free Use Bible API provides Bible translations in JSON format. The API itself is licensed under the MIT License. Individual Bible translations may have their own licensing terms, which are specified by the respective translation providers.

Biblos does not claim ownership of the Bible texts. The copyright and licensing terms of each translation remain with their respective copyright holders.

For more information about the API and the licensing of individual translations, see the [Free Use Bible API documentation](https://bible.helloao.org/docs/) and the [API reference](https://bible.helloao.org/docs/reference/).
