#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ADD_PDF     "\\includepdf[pages=-]{%s}\n"
#define EXT_SIZE    4
#define FILES_START 2
#define FMT_SIZE    1
#define NO_EXIST    -1
#define OUT_DIR     1
#define PDF_EXT     ".pdf", 4
#define POSTAMBLE   "\\end{document}\n"
#define PREAMBLE1   "\\documentclass[english,10pt]{article}\n"
#define PREAMBLE2   "\\usepackage{pdfpages}\n\\begin{document}\n"
#define TEMPLATE    "%s/template.tex"
#define WRITE       "w"

typedef enum Status {
    GOOD    = 0,
    BAD_PDF = 1
} Status;

void        fill_template   (char *,    int,    char ** );
void        kill_filenames  (int,       char **	        );
char    *   asprintf        (char *,    char *          );
char    *   cut_name        (char *                     );
char    *   extend          (char *                     );
char    **  rectify         (int,       char **         );
Status      validate        (int,       char **         );

int main(int argc, char **argv) {
    Status  status;
    char    *tmpdir, **filenames;
    int     noFiles;

    noFiles     = argc - FILES_START;
    filenames   = rectify(noFiles, &(argv[FILES_START]));
    status      = validate(noFiles, filenames);
    if (status == GOOD) fill_template(argv[OUT_DIR], noFiles, filenames);
    kill_filenames(noFiles, filenames);

    return (status);
}

void fill_template(char *tempdir, int pdfs, char **filenames) {
    FILE    *template;
    char    *add, *cutName, *tempFname;
    int     pdf;

    tempFname   = asprintf(TEMPLATE, tempdir);
    template    = fopen(tempFname, WRITE);

    fprintf(template, PREAMBLE1);
    fprintf(template, PREAMBLE2);
    for (pdf    = 0; pdf < pdfs; pdf++) {
        cutName = cut_name(filenames[pdf]);
        add     = asprintf(ADD_PDF, cutName);
        fprintf(template, add);
        free(add);
        free(cutName);
    }
    fprintf(template, POSTAMBLE);

    fclose(template);
    free(tempFname);
}

void kill_filenames(int files, char **filenames) {
    int kill;

    for (kill = 0; kill < files; kill++) free(filenames[kill]);
    free(filenames);
}

char *asprintf(char *template, char *insert) {
    int     totlen;
    char    *out;

    totlen  = strlen(template) + strlen(insert) - FMT_SIZE;
    out     = calloc(sizeof(char), totlen);
    sprintf(out, template, insert);

    return (out);
}

char *cut_name(char *filename) {
    int     cutLen;
    char    *outName;

    cutLen  = strlen(filename) - EXT_SIZE;
    outName = calloc(sizeof(char), cutLen + 1);
    strncpy(outName, filename, cutLen);

    return (outName);
}

char *extend(char *filename) {
    char    *outname;
    int     base;

    base    = strlen(filename);
    outname = calloc(sizeof(char), (base + EXT_SIZE));
    strcpy(outname, filename);
    strncpy(&(outname[base]), PDF_EXT);

    return (outname);
}

char **rectify(int noFiles, char **cmdlineNames) {
    char    test,   *extens, *inName, *outName, **fixedNames;
    int     check,  extInd;

    fixedNames  = calloc(sizeof(char *), noFiles);
    for (check  = 0; check < noFiles; check++) {
        inName  = cmdlineNames[check];
        extInd  = strlen(inName) - EXT_SIZE;
        extens  = &(inName[extInd]);
        outName = !strncmp(extens, PDF_EXT) ?
                strdup(inName) : extend(inName);
        fixedNames[check] = outName;
    }

    return (fixedNames);
}

Status validate(int noFiles, char **pdfs) {
    Status  status;
    int     check;
    char    *pdf;

    status = GOOD;
    for (check = 0; ((check < noFiles) && (status == GOOD)); check++) {
        pdf = pdfs[check];
        if (access(pdf, R_OK) == NO_EXIST) status = BAD_PDF;
    }

    return (status);
}
