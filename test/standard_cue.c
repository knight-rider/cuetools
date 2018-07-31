#include <stdio.h>
#include <string.h>

#include "libcue.h"
#include "minunit.h"

int tests_run;

/* Frames per second */
#define FPS (75)
#define MSF_TO_F(m,s,f) ((f) + ((m)*60 + (s))*FPS)

static char cue[] =   "REM GENRE Alternative\n"
                      "REM DATE 1991\n"
                      "REM DISCID 860B640B\n"
                      "REM COMMENT \"ExactAudioCopy v0.95b4\"\n"
                      "PERFORMER \"My Bloody Valentine\"\n"
                      "TITLE \"Loveless\"\n"
                      "FILE \"My Bloody Valentine - Loveless.wav\" WAVE\n"
                        "TRACK 01 AUDIO\n"
                           "TITLE \"Only Shallow\"\n"
                           "PERFORMER \"My Bloody Valentine\"\n"
                           "INDEX 01 00:00:00\n"
                        "TRACK 02 AUDIO\n"
                           "TITLE \"Loomer\"\n"
                           "PERFORMER \"My Bloody Valentine\"\n"
                           "INDEX 01 04:17:52\n";

static char* cue_test()
{
   struct Cd *cd = cue_parse_string(cue);
   mu_assert ("error parsing CUE", cd != NULL);

   struct Cdtext *cdtext = cd_get_cdtext(cd);
   mu_assert ("error getting CDTEXT", cdtext != NULL);

   const char *val;
   val = cdtext_get(cdtext, PTI_PERFORMER);
   mu_assert("error getting CD performer", val != NULL);
	printf("performer: %s\n", val);
   mu_assert("error validating CD performer", strcmp (val, "My Bloody Valentine") == 0);

   val = cdtext_get(cdtext, PTI_TITLE);
   mu_assert("error getting CD title", val != NULL);
	printf("title: %s\n", val);
   mu_assert("error validating CD title", strcmp (val, "Loveless") == 0);

   val = cdtext_get(cdtext, PTI_GENRE);
   mu_assert("error getting CD genre", val != NULL);
	printf("genre: %s\n", val);
   mu_assert("error validating CD genre", strcmp (val, "Alternative") == 0);

   val = rem_get(cdtext, REM_DATE);
   mu_assert("error getting CD date", val != NULL);
	printf("date: %s\n", val);
   mu_assert("error validating CD date", strcmp (val, "1991") == 0);

   int ival = cd_get_ntrack(cd);
   mu_assert("invalid number of tracks", ival == 2);
	printf("ntrack: %d\n", ival);

   struct Track *track;
   /* Track 1 */
   track = cd_get_track(cd, 1);
   mu_assert("error getting track", track != NULL);

   val = track_get_filename(track);
   mu_assert("error getting track filename", val != NULL);
	printf("track fname: %s\n", val);
   mu_assert("error validating track filename", strcmp (val, "My Bloody Valentine - Loveless.wav") == 0);

   cdtext = track_get_cdtext(track);
   mu_assert("error getting track CDTEXT", cdtext != NULL);

   val = cdtext_get(cdtext, PTI_PERFORMER);
   mu_assert("error getting track performer", val != NULL);
	printf("track performer: %s\n", val);
   mu_assert("error validating track performer", strcmp (val, "My Bloody Valentine") == 0);

   val = cdtext_get(cdtext, PTI_TITLE);
   mu_assert("error getting track title", val != NULL);
	printf("track title: %s\n", val);
   mu_assert("error validating track title", strcmp (val, "Only Shallow") == 0);

   ival = track_get_start(track);
   mu_assert("invalid track start", ival == 0);
	printf("track start: %d\n", ival);
   ival = track_get_length(track);
   mu_assert("invalid track length", ival == MSF_TO_F(4,17,52));
	printf("track length: %d\n", ival);

   ival = track_get_index(track, 1);
   mu_assert("invalid index", ival == 0);
	printf("track index: %d\n", ival);

   /* Track 2 */
   track = cd_get_track(cd, 2);
   mu_assert("error getting track", track != NULL);

   val = track_get_filename(track);
   mu_assert("error getting track filename", val != NULL);
   mu_assert("error validating track filename", strcmp (val, "My Bloody Valentine - Loveless.wav") == 0);

   cdtext = track_get_cdtext(track);
   mu_assert("error getting track CDTEXT", cdtext != NULL);

   val = cdtext_get(cdtext, PTI_PERFORMER);
   mu_assert("error getting track performer", val != NULL);
   mu_assert("error validating track performer", strcmp (val, "My Bloody Valentine") == 0);

   val = cdtext_get(cdtext, PTI_TITLE);
   mu_assert("error getting track title", val != NULL);
   mu_assert("error validating track title", strcmp (val, "Loomer") == 0);

   ival = track_get_start(track);
   mu_assert("invalid track start", ival == MSF_TO_F(4,17,52));
   ival = track_get_length(track);
   mu_assert("invalid track length", ival == -1);

   ival = track_get_index(track, 1);
   mu_assert("invalid index", ival == MSF_TO_F(4,17,52));

   cd_free(cd);

   return NULL;
}

static char* run_tests()
{
   mu_run_test (cue_test);
   return NULL;
}

int main (int argc, char **argv)
{
   char *result = run_tests();
   if (result)
      printf ("%s\n", result);
   else
      printf ("All tests passed!\n");

   printf ("Tests run: %d\n", tests_run);

   return result != NULL;
}
