#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "CorCalls.h"
#include "PagePDECntCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "CosCalls.h"
#include "AcroColorCalls.h"
#include <sys/types.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <libgen.h>
#include <stdio.h>

using namespace std;

AC_Profile makeColorProfile(string profileName) {
  ASFile Profile;
  ASTFilePos ProfileSize;
  ASUns8 *ProfileBuffer;
  AC_Profile outputProfile;

  size_t bufsize = PATH_MAX;
  char* buf = (char*)malloc(bufsize);
  int buflen = 0;
  buflen = readlink("/proc/self/exe", buf, bufsize);

  char execPath[buflen + 1];
  memcpy(&execPath, buf, buflen);
  execPath[bufsize] = '\0';
  char* execFolder  = dirname(execPath);

  string profilePath = string(execFolder) + "/Resources/Color/Profiles/" + profileName + ".icc";
  char *cprofilePath = (char *) profilePath.c_str();

  ASPathName Path = ASFileSysCreatePathFromCString(NULL, cprofilePath);

  ASFileSysOpenFile(NULL, Path, ASFILE_READ, &Profile);

  ProfileSize = ASFileGetEOF(Profile);
  ProfileBuffer = (ASUns8 *) ASmalloc(ProfileSize);

  ASFileRead(Profile, (char *) ProfileBuffer, ProfileSize);
  ASFileClose(Profile);

  ACMakeBufferProfile(&outputProfile, ProfileBuffer, ProfileSize);

  ASfree(ProfileBuffer);

  return outputProfile;
}

PDColorConvertParamsEx makeColorConvertParams(AC_Profile profile) {
  PDColorConvertParamsEx colorParams = (PDColorConvertParamsEx) calloc(1, sizeof(PDColorConvertParamsRecEx));
  PDColorConvertActionEx colorConvertAction = (PDColorConvertActionEx) calloc(1, sizeof(PDColorConvertActionRecEx));

  colorConvertAction->mMatchAttributesAny = kColorConvObj_AnyObject;
  colorConvertAction->mMatchSpaceTypeAny = kColorConvRGBSpace;
  colorConvertAction->mMatchIntent = AC_UseProfileIntent;
  colorConvertAction->mAction = kColorConvConvert;
  colorConvertAction->mEmbed = false;
  colorConvertAction->mPreserveBlack = true;
  colorConvertAction->mPromoteGrayToCMYK = true;
  colorConvertAction->mPreserveCMYKPrimaries = true;
  colorConvertAction->mConvertProfile = profile;

  colorParams->mActions = colorConvertAction;
  colorParams->mNumActions = 1;
  colorParams->mInks = NULL;
  colorParams->mNumInks = 0;

  return colorParams;
}

void releaseTempFiles(PDPage &page) {
  PDPageAcquirePDEContent(page, 0);
  PDPageReleasePDEContent(page, 0);
  PDPageRelease(page);
}

int main(int argc, char **argv) {
  APDFLib libInit;
  ASErrorCode errCode = 0;

  if (libInit.isValid() == false) {
    return libInit.getInitError();
  }

  const string inputFilename = argv[1];
  const string outputFilename = argv[2];

  DURING

  printf("Creating profile...\n");
  AC_Profile profile = makeColorProfile("USWebCoatedSWOP");
  printf("Profile created!\n");

  printf("Creating color conversion params...\n");
  PDColorConvertParamsEx colorConvertParams = makeColorConvertParams(profile);
  printf("Color conversion params created!\n");

  printf("Creating doc from file: %s\n", inputFilename.c_str());
  APDFLDoc *doc = new APDFLDoc(inputFilename.c_str(), true);
  printf("Doc created!\n");

  printf("[INFO] Converting to CMYK - STARTING\n");

  printf("[INFO] doc has %lu pages\n", doc->numPages());
  ASSize_t i = 0;
  while(i < doc->numPages()) {
    printf("Converting page %lu\n", i);
    ASBool changed;
    PDDoc pdDoc = doc->getPDDoc();
    printf("Have pdDoc; going to convert page to CMYK\n");
    ASBool failed = PDDocColorConvertPageEx(pdDoc, colorConvertParams, i, NULL, NULL, 0, NULL, &changed);
    printf("Page converted?\n");


    PDPage page = doc->getPage(i);
    releaseTempFiles(page);

    if (failed) {
      printf("[INFO] Converting to CMYK - FAILED, EXITING...\n");
      break;
    }
    i++;
  }
  free(colorConvertParams);
  ACUnReferenceProfile(profile);
  printf("[INFO] Converting to CMYK - DONE\n");

  // 3. Save the document
  doc->saveDoc(outputFilename.c_str());



  HANDLER

  errCode = ERRORCODE;
  libInit.displayError(errCode);

  END_HANDLER
  return errCode;

}
