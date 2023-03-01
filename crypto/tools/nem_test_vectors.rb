#!/usr/bin/env ruby
require 'highline'
require 'open-uri'

TEMPLATE_NAME = (Pathname.new(__FILE__).sub_ext '.erb').to_s.freeze

@terminal = HighLine.new($stdin, $stderr)

def github_files
  require 'octokit'

  @github_files ||= Octokit.contents('NemProject/nem-test-vectors')
                           .select do |file|
    file.name.end_with? '.dat'
  end
end

def choose_data_file
  @terminal.choose do |menu|
    github_files.each do |file|
      menu.choice(file.name) { file.download_url }
    end

    menu.prompt = 'Which file?  '

    menu.index = :none
    menu.select_by = :name
  end
end

def load_header(line)
  line = line.dup
  abort 'Header is not a comment' unless line.slice!(0) == '#'

  header = line.split(':').each(&:strip!)
  header.shift if header.first.empty?

  header
end

def parse_field_answer(answer)
  if answer.empty?
    nil
  elsif /^(?:(?<type>\w+) )?(?<identifier>\w+)$/ =~ answer
    [identifier, type]
  else
    raise NotValidQuestionError
  end
end

def ask_fields(header)
  header.map do |name|
    @terminal.ask "Field for `#{name}'?  " do |question|
      question.answer_type = lambda(&method(:parse_field_answer))
    end
  end
end

def load_data_line(line)
  abort 'Line does not begin with colon' unless line.slice!(0) == ':'

  line.strip!
  line.chomp!(',')

  values = line.split(':').each(&:strip!)
  values.pop if values.last.empty?

  values
end

def load_data(file, count)
  file.each_line.lazy.reject { |line| line.start_with? '#' }
      .take(count)
      .map(&method(:load_data_line))
      .to_a
end

def remove_skipped_fields(fields, data)
  data.each do |values|
    abort 'Line does not match header' unless values.size == fields.size

    values.reject!.each_with_index { |_, index| fields[index].nil? }
  end

  fields.compact!
end

def format_data_fields(fields, data)
  data.each do |values|
    fields.each_with_index do |(_, type), index|
      values[index] = values[index].dump if type.nil?
    end
  end
end

def template(source_url, fields, data)
  test_name = @terminal.ask('Name for test?  ') do |question|
    question.validate = /./
  end

  erb = ERB.new(File.read(TEMPLATE_NAME), nil, '-')
  erb.filename = TEMPLATE_NAME

  erb.result(binding)
end

download_url = choose_data_file

source_code = open download_url do |file|
  line = file.readline

  header = load_header(line)
  @terminal.say line

  count = @terminal.ask('How many vectors to import?  ', Integer)

  fields = ask_fields(header)
  data = load_data(file, count)

  remove_skipped_fields(fields, data)
  format_data_fields(fields, data)

  template(download_url, fields, data)
end

puts source_code
