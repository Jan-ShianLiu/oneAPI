@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!perl
#line 15
    eval 'exec d:\arc\perl\bin\perl.exe -S $0 ${1+"$@"}'
	if $running_under_some_shell;
#!./perl
use 5.008001;
BEGIN { pop @INC if $INC[-1] eq '.' }
use strict;
use warnings;
use Encode;
use Getopt::Std;
use Carp;
use Encode::Guess;
$Getopt::Std::STANDARD_HELP_VERSION = 1;

my %opt;
getopts( "huSs:", \%opt );
my @suspect_list;
list_valid_suspects() and exit if $opt{S};
@suspect_list = split /:,/, $opt{s} if $opt{s};
HELP_MESSAGE() if $opt{h};
HELP_MESSAGE() unless @ARGV;
do_guess($_) for @ARGV;

sub read_file {
    my $filename = shift;
    local $/;
    open my $fh, '<:raw', $filename or croak "$filename:$!";
    my $content = <$fh>;
    close $fh;
    return $content;
}

sub do_guess {
    my $filename = shift;
    my $data     = read_file($filename);
    my $enc      = guess_encoding( $data, @suspect_list );
    if ( !ref($enc) && $opt{u} ) {
        return 1;
    }
    print "$filename\t";
    if ( ref($enc) ) {
        print $enc->mime_name();
    }
    else {
        print "unknown";
    }
    print "\n";
    return 1;
}

sub list_valid_suspects {
    print join( "\n", Encode->encodings(":all") );
    print "\n";
    return 1;
}

sub HELP_MESSAGE {
    exec 'pod2usage', $0 or die "pod2usage: $!" 
}
__END__
=head1 NAME

encguess - guess character encodings of files

=head1 VERSION

$Id: encguess,v 0.2 2016/08/04 03:15:58 dankogai Exp $

=head1 SYNOPSIS

  encguess [switches] filename...

=head2 SWITCHES

=over 2

=item -h

show this message and exit.

=item -s

specify a list of "suspect encoding types" to test, 
seperated by either C<:> or C<,>

=item -S

output a list of all acceptable encoding types that can be used with
the -s param

=item -u

suppress display of unidentified types

=back

=head2 EXAMPLES:

=over 2

=item *

Guess encoding of a file named C<test.txt>, using only the default
suspect types.

   encguess test.txt

=item *

Guess the encoding type of a file named C<test.txt>, using the suspect
types C<euc-jp,shiftjis,7bit-jis>.

   encguess -s euc-jp,shiftjis,7bit-jis test.txt
   encguess -s euc-jp:shiftjis:7bit-jis test.txt

=item *

Guess the encoding type of several files, do not display results for
unidentified files.

   encguess -us euc-jp,shiftjis,7bit-jis test*.txt

=back

=head1 DESCRIPTION

The encoding identification is done by checking one encoding type at a
time until all but the right type are eliminated. The set of encoding
types to try is defined by the -s parameter and defaults to ascii,
utf8 and UTF-16/32 with BOM. This can be overridden by passing one or
more encoding types via the -s parameter. If you need to pass in
multiple suspect encoding types, use a quoted string with the a space
separating each value.

=head1 SEE ALSO

L<Encode::Guess>, L<Encode::Detect>

=head1 LICENSE AND COPYRIGHT

Copyright 2015 Michael LaGrasta and Dan Kogai.

This program is free software; you can redistribute it and/or modify it
under the terms of the the Artistic License (2.0). You may obtain a
copy of the full license at:

L<http://www.perlfoundation.org/artistic_license_2_0>

=cut

__END__
:endofperl
