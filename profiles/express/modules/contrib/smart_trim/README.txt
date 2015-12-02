Smart Trim implements a new field formatter for textfields (text, text_long,
  and text_with_summary, if you want to get technical) that improved upon the
  "Summary or Trimmed" formatter built into Drupal 7.

After installing and enabling Smart Trim, you should see a "Smart trimmed"
  option in the format dropdown for your text fields. With smart trim, you have
  control over:
    1. The trim length
    2. Whether the trim length is measured in characters or word
    3. Appending an optional suffix at the trim point
    4. Displaying an optional "More" link immediately after the trimmed text.
    5. Stripping out HTML tags from the field

The "More" link functionality may not make sense in many contexts, and may be
 redundant in situations where "Read More" is included in set of links included
 with the node.

Initial release is strictly for Drupal 7. No backport to Drupal 6 is planned.

Note that HTML markup not seen by end-users will still be counted when
  calculating trim length. This may be addressed in future releases.

Smart Trim was initially developed by Ben Byrne while at New Signature
  (bbyrne@newsignature.com) but Ben is now at Cornershop Creative
  (ben@cornershopcreative.com)