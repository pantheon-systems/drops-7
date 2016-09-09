
This documentation is a combination of the official [Oracle Enrollment Web Services Users Guide documentation](https://github.com/CuBoulder/cu_classes_bundle/blob/2a2/Enrollment%20Web%20Services%20Users%20Guide.pdf), information provided by UIS, and what we've learned through trial and error.  

It is very unlikely that this documenation is or will ever will be 100% accurate.  It is provided without warranty to help anyone else attempting to work this API.  

##SSR_GET_CLASSES

Allows the student to initiate a search of classes and view the search results.

The Class search service can be used to search for the classes based on the input search criteria specified by the user. The service operation also takes in additional search criteria as parameters as in the current SS. Along with the class search result, the response also includes the following: SSR_COURSE_COUNT- Count of courses matching the search criteria. SSR_CLASS_COUNT - Count of class section matching the search criteria. SSR_ERR_LMT_EXCEED - Set to Y if search result exceeds error limit specified in the installation table
SSR_WRN_LMT_EXCEED - Set to Y if search result exceeds warning limit specified in the installation table and obey_warning_limit in the request payload is set to YES.

##ERROR HANDLING

The following conditions result in a service error:
* The supplied input parameters are invalid
* The service operation obeys the Term values setup as defined in the
Term Values table. So if the Term is set not to be available for self
service users, then an error message is shown.
* The service operation obeys the Error/Warning limit as set in Student
Record Installation set up. If the SSR_CLASS_COUNT is not within
the error limit, then an error message is shown in the response.
* If it is not within the warning limit and the Obey Warning Limit in the
request is Y, then a warning message is displayed in the response along with the class search results. It is up to the UI to either show or not show the class search results in case of warning.

##END POINTS

##VIEWS

##POST VALUES

The only required post values are; *INSTITUTION*, *STRM*, and *SUBJECT*

Additional values include:

```xml
<CLASS_SEARCH_REQUEST>
  <INSTITUTION></INSTITUTION> 
  <STRM></STRM>
  <SUBJECT></SUBJECT>
  <CRSE_ID></CRSE_ID> 
  <CLASS_TYPE></CLASS_TYPE>
  <CU_CLASS_STAT></CU_CLASS_STAT>
  <CRSE_ATTR></CRSE_ATTR> 
  <CRSE_ATTR_VALUE></CRSE_ATTR_VALUE>
  <SCHEDULE_PRINT></SCHEDULE_PRINT> 
  <ACAD_ORG></ACAD_ORG> 
  <ACAD_GROUP></ACAD_GROUP>
  <CLASS_NBR></CLASS_NBR>
  <CRSE_OFFER_NBR></CRSE_OFFER_NBR>
  <SESSION_CODE></SESSION_CODE>
  <CLASS_SECTION></CLASS_SECTION>
  <CATALOG_NBR></CATALOG_NBR>
  <SSR_EXACT_MATCH1></SSR_EXACT_MATCH1>
  <SSR_OPEN_ONLY>N</SSR_OPEN_ONLY>
  <OEE_IND></OEE_IND>
  <DESCR></DESCR>
  <ACAD_CAREER></ACAD_CAREER>
  <SSR_COMPONENT></SSR_COMPONENT>
  <INSTRUCTION_MODE></INSTRUCTION_MODE>
  <CAMPUS></CAMPUS>
  <LOCATION></LOCATION>
  <MEETING_TIME_START></MEETING_TIME_START>
  <SSR_MTGTIME_START2></SSR_MTGTIME_START2>
  <MEETING_TIME_END></MEETING_TIME_END>
  <MON>N</MON>
  <TUES>N</TUES>
  <WED>N</WED>
  <THURS>N</THURS>
  <FRI>N</FRI>
  <SAT>N</SAT>
  <SUN>N</SUN>
  <INCLUDE_CLASS_DAYS></INCLUDE_CLASS_DAYS>
  <LAST_NAME></LAST_NAME>
  <SSR_EXACT_MATCH2></SSR_EXACT_MATCH2>
  <UNITS_MINIMUM></UNITS_MINIMUM>
  <UNITS_MAXIMUM></UNITS_MAXIMUM>
  <SCC_ENTITY_INST_ID></SCC_ENTITY_INST_ID>
  <OBEY_WARNING_LIMIT></OBEY_WARNING_LIMIT>
  <SSR_START_TIME_OPR></SSR_START_TIME_OPR>
  <SSR_END_TIME_OPR></SSR_END_TIME_OPR>
  <SSR_UNITS_MIN_OPR></SSR_UNITS_MIN_OPR>
  <SSR_UNITS_MAX_OPR></SSR_UNITS_MAX_OPR>
</CLASS_SEARCH_REQUEST>
```

*INSTITUTION* - Allowed values are CUBLD, CUSPG, or CUDEN.

*STRM* - The name and numberic code of these are available from an LOV Service as they are available, but they use the following pattern:

* 2154 = 2015 Summer
* 2157 = 2015 Fall
* 2161 = 2016 Spring
* 2164 = 2016 Summer
* 2167 = 2016 Fall

*SUBJECT* - Available from an LOV Service

*CRSE_ID* - Requests that include SUBJECT with no CRSE_ID will return all courses for the SUBJECT

*CLASS_TYPE* - E or N (Enroll or Not Enroll)

*CU_CLASS_STAT* - A (default), X only cancelled classes, AX both active and cancelled

*SCHEDULE_PRINT* - Y or N

*ACAD_CAREER* - UGRD

*LAST_NAME* - 

*CAMPUS* - Each institution has a main and continuing education campus.  The campus codes for Boulder are BLDR and CEPS.

*INSTRUCTION_MODE* - See https://github.com/CuBoulder/cu_classes_bundle/issues/17

EXAMPLE XML RESPONSE

```xml
<?xml version="1.0"?>
  <SSR_GET_CLASSES_RESP xmlns = "http://xmlns.oracle.com/Enterprise/HCM/services">
  <SEARCH_RESULT>
  <ERROR_W ARN_TEXT></ERROR_W ARN_TEXT> 
  <SSR_COURSE_COUNT>1</SSR_COURSE_COUNT> 
  <SSR_ERR_LMT_EXCEED></SSR_ERR_LMT_EXCEED> 
  <SSR_WRN_LMT_EXCEED></SSR_WRN_LMT_EXCEED>
  <SSR_CLASS_COUNT>9</SSR_CLASS_COUNT> 
  <SUBJECTS>
    <SUBJECT>
      <CRSE_ID>007116</CRSE_ID>
      <CRSE_ID_LOVDescr>Psychology Special Topics</CRSE_ID_LOVDescr>
      <SUBJECT>PSYCH</SUBJECT> <SUBJECT_LOVDescr>Psychology</SUBJECT_LOVDescr>
      <CATALOG_NBR>495</CATALOG_NBR>
      <INSTITUTION>PSUNV</INSTITUTION>
      <INSTITUTION_LOVDescr>PeopleSoft University</INSTITUTION_LOVDescr>
      <ACAD_CAREER>UGRD</ACAD_CAREER>
      <ACAD_CAREER_LOVDescr>Undergraduate</ACAD_CAREER_LOVDescr>
      <COURSE_TITLE_LONG>Psychology Special Topics</COURSE_TITLE_LONG>
      <CLASSES_SUMMARY>
        <CLASS_SUMMARY>
          <CRSE_ID>007116</CRSE_ID>
          <CRSE_ID_LOVDescr>Psychology Special Topics</CRSE_ID_LOVDescr>
          <SUBJECT>PSYCH</SUBJECT>
          <SUBJECT_LOVDescr>Psychology</SUBJECT_LOVDescr>
          <CATALOG_NBR>495</CATALOG_NBR> <CRSE_OFFER_NBR>1</CRSE_OFFER_NBR>
          <STRM>0430</STRM>
          <STRM_LOVDescr>2001 Spring</STRM_LOVDescr>
          <SESSION_CODE>1</SESSION_CODE>
          <SESSION_CODE_LOVDescr>Regular Academic Session</SESSION_CODE_LOVDescr>
          <CLASS_SECTION>01A</CLASS_SECTION>
          <CLASS_NBR>1298</CLASS_NBR>
          <SCHEDULE_PRINT>Y</SCHEDULE_PRINT>
          <SCHEDULE_PRINT_LOVDescr>Y es</SCHEDULE_PRINT_LOVDescr>
          <COMBINED_SECTION></COMBINED_SECTION>
          <COMBINED_SECTION_LOVDescr></COMBINED_SECTION_LOVDescr>
          <CLASS_TOPIC></CLASS_TOPIC>
          <SSR_CLASSNAME_LONG></SSR_CLASSNAME_LONG>
          <STATUS>O</STATUS>
          <STATUS_LOVDescr>Open</STATUS_LOVDescr>
          <SSR_COMPONENT>LAB</SSR_COMPONENT>
          <SSR_COMPONENT_LOVDescr>Laboratory</SSR_COMPONENT_LOVDescr>
          <CLASSES_MEETING_PATTERNS>
            <CLASS_MEETING_PATTERN>
              <SSR_MTG_SCHED_LONG>Fr 8:00AM - 10:30AM</SSR_MTG_SCHED_LONG>
              <SSR_MTG_LOC_LONG>TBA</SSR_MTG_LOC_LONG>
              <SSR_INSTR_LONG>Mara Baylor</SSR_INSTR_LONG>
              <SSR_MTG_DT_LONG>01/05/2001 - 05/05/2001</SSR_MTG_DT_LONG>
              <SSR_TOPIC_LONG>TBA</SSR_TOPIC_LONG>
            </CLASS_MEETING_PATTERN>
          </CLASSES_MEETING_PATTERNS>
        </CLASS_SUMMARY> 
        <CLASS_SUMMARY>
          <CRSE_ID>007116</CRSE_ID>
          <CRSE_ID_LOVDescr>Psychology Special Topics</CRSE_ID_LOVDescr>
          <SUBJECT>PSYCH</SUBJECT>
          <SUBJECT_LOVDescr>Psychology</SUBJECT_LOVDescr>
          <CATALOG_NBR>495</CATALOG_NBR>
          <CRSE_OFFER_NBR>1</CRSE_OFFER_NBR>
          <STRM>0430</STRM>
          <STRM_LOVDescr>2001 Spring</STRM_LOVDescr>
          <SESSION_CODE>1</SESSION_CODE>
          <SESSION_CODE_LOVDescr>Regular Academic Session</SESSION_CODE_LOVDescr>
          <CLASS_SECTION>04A</CLASS_SECTION>
          <CLASS_NBR>1362</CLASS_NBR>
          <SCHEDULE_PRINT>Y</SCHEDULE_PRINT>
          <SCHEDULE_PRINT_LOVDescr>Yes</SCHEDULE_PRINT_LOVDescr>
          <COMBINED_SECTION></COMBINED_SECTION>
          <COMBINED_SECTION_LOVDescr></COMBINED_SECTION_LOVDescr>
          <CLASS_TOPIC></CLASS_TOPIC>
          <SSR_CLASSNAME_LONG></SSR_CLASSNAME_LONG>
          <STATUS>O</STATUS>
          <STATUS_LOVDescr>Open</STATUS_LOVDescr>
          <SSR_COMPONENT>LAB</SSR_COMPONENT>
          <SSR_COMPONENT_LOVDescr>Laboratory</SSR_COMPONENT_LOVDescr> 
          <CLASSES_MEETING_PATTERNS>
            <CLASS_MEETING_PATTERN>
              <SSR_MTG_SCHED_LONG>Fr 9:00AM - 11:30AM</SSR_MTG_SCHED_LONG>
              <SSR_MTG_LOC_LONG>TBA</SSR_MTG_LOC_LONG>
              <SSR_INSTR_LONG>Staff</SSR_INSTR_LONG>
              <SSR_MTG_DT_LONG>01/16/2001 - 05/18/2001</SSR_MTG_DT_LONG>
              <SSR_TOPIC_LONG>TBA</SSR_TOPIC_LONG>
            </CLASS_MEETING_PATTERN> 
          </CLASSES_MEETING_PATTERNS>
        </CLASS_SUMMARY>
    </CLASSES_SUMMARY>
  </SUBJECT> 
</SUBJECTS>
</SEARCH_RESULT> 
</SSR_GET_CLASSES_RESP>
```
