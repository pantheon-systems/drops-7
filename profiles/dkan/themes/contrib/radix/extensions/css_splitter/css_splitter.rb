# Splits stylesheets that go beyond the IE limit of 4095 selectors.
# See https://github.com/zweilove/css_splitter
Encoding.default_external = Encoding::UTF_8
Encoding.default_internal = Encoding::UTF_8
class CssSplitter

  def self.split(infile, outdir = File.dirname(infile), max_selectors = 4095)

    raise "infile could not be found" unless File.exists? infile

    rules = IO.readlines(infile, "}")
    return if rules.first.nil?

    charset_statement, rules[0] = rules.first.partition(/^\@charset[^;]+;/)[1,2]
    return if rules.nil?

    # The infile remains the first file
    selectors_count = 0
    output = nil

    rules.each do |rule|
      rule_selectors_count = count_selectors_of_rule rule
      selectors_count += rule_selectors_count

      # Nothing happens until the selectors limit is reached for the first time
      if selectors_count > max_selectors
        # Close current file if there is already one
        output.close if output

        # Prepare next file
        filename = File.join(outdir, "ie.css")
        output = File.new(filename, "w")
        output.write charset_statement

        # Reset count with current rule count
        selectors_count = rule_selectors_count
      end

      output.write rule if output
    end
  end

  def self.count_selectors(css_file)
    raise "file could not be found" unless File.exists? css_file

    rules = IO.readlines(css_file, '}')
    return if rules.first.nil?

    charset_statement, rules[0] = rules.first.partition(/^\@charset[^;]+;/)[1,2]
    return if rules.first.nil?

    rules.inject(0) {|count, rule| count + count_selectors_of_rule(rule)}.tap do |result|
      puts File.basename(css_file) + " contains #{result} selectors."
    end
  end

  def self.count_selectors_of_rule(rule)
    rule.partition(/\{/).first.scan(/,/).count.to_i + 1
  end

end