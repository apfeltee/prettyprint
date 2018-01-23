#!/usr/bin/ruby

tpl = <<__EOF__
        template<>
        std::string tname<%s>()
        {
            return %p;
        }
__EOF__

types = %w(
  short int char wchar_t float double long
  std::string std::wstring
)
types.push("long long")
types.each do |t|
  $stdout.printf(tpl, t, t)
  $stdout.puts("\n")
end

