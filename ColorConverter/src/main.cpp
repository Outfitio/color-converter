#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "CorCalls.h"
#include "AcroColorCalls.h"
#include "PagePDECntCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "CosCalls.h"
#include <stdio.h>

using namespace std;

AC_Profile makeColorProfile(string profileName) {
  ASFile Profile;
  ASTFilePos ProfileSize;
  ASUns8 *ProfileBuffer;
  AC_Profile outputProfile;
  string profilePath = "./Resources/Color/Profiles/" + profileName + ".icc";
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
  PDColorConvertParamsEx colorParams = (PDColorConvertParamsEx) malloc(sizeof(PDColorConvertParamsRecEx));
  PDColorConvertActionEx colorConvertAction = (PDColorConvertActionEx) malloc(sizeof(PDColorConvertActionRecEx));

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

  DURING

  AC_Profile profile = makeColorProfile("USWebCoatedSWOP");
  PDColorConvertParamsEx colorConvertParams = makeColorConvertParams(profile);
  APDFLDoc *doc = new APDFLDoc(argv[1], true);

  printf("[INFO] Converting to CMYK - STARTING\n");
  int i = 0;
  while(i < doc->numPages()) {
    ASBool failed = PDDocColorConvertPageEx(doc->getPDDoc(), colorConvertParams, i, NULL, NULL, 0, NULL, NULL);

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
  doc->saveDoc(argv[2]);

  HANDLER

  errCode = ERRORCODE;
  libInit.displayError(errCode);

  END_HANDLER
  return errCode;

}
