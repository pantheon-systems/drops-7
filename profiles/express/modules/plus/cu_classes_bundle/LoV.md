# I'm LoVin' It - LoV Lookup Service
The Lookup of Value (LoV) service is provided by University Information Systems (UIS), and it allows you to search an organizational tree for class subject values. 

## Authorization
You will need to get a username and password from UIS to use the LoV service. 

## Organizational Tree
The LoV service has two endpoints, one for organizations and the other for subjects. Organizations can be an institution, a college, a department, or whatever else might be lurking in the Oracle database. You can use this endpoint to dive down into a tree for codes that you can then use to lookup subjects. 
```
esbprd.prod.cu.edu/All_Cs_Lov_OrgTree?root=
```
The root query parameter is where you add your organizational code. An example of a tree:
```
https://esbprd.prod.cu.edu/All_Cs_Lov_OrgTree?root=
# Returns 'B-CUBLD' a campus code.
https://esbprd.prod.cu.edu/All_Cs_Lov_OrgTree?root=B-CUBLD
# Returns 'B-ARSC' a college.
https://esbprd.prod.cu.edu/All_Cs_Lov_OrgTree?root=B-ARSC
# Returns 'B-ASAH' a department.
https://esbprd.prod.cu.edu/All_Cs_Lov_OrgTree?root=B-ASAH
# Returns 'B-CAMW' a center. 
https://esbprd.prod.cu.edu/All_Cs_Lov_OrgTree?root=B-CAMW
# Returns nothing. The orgTree has ended. 
```
All of the organizational codes are prefixed with a campus letter to avoid duplicate codes from two different campuses, e.g. "B-ARSC" vs. "C-ARSC".

## Subject List
Once you have your organizational codes from the "OrgTree" endpoint, then you can use those values to obtain subject lists. All of the subject codes won't be prefixed with a campus letter. 

```
https://esbprd.prod.cu.edu/All_Cs_Lov_Subject?root=B-CAMW
# Returns 'CAMW' a subject code. 
```

