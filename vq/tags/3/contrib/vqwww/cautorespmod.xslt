<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/autoresp">Automatyczna odpowied�</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/autoresp">
Tutaj mo�esz ustawi� odpowied�, kt�ra b�dzie automatycznie wysy�ana
do nadawc�w otrzymywanych wiadomo�ci.
</xsl:template>

<xsl:template match="vqwww/mod/autoresp">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie mo�na pobra� aktualnie ustawionej wiadomo�ci.</td></tr>
</xsl:when>
<xsl:otherwise>

<!-- error messages -->
<xsl:if test="chgcant">
<tr><td colspan="2">Nie mo�na zmieni� wiadomo�ci.</td></tr>
</xsl:if>

<!-- input form -->
<tr><td>Poni�ej znajduje si� tre�� odsy�anej wiadomo�ci:</td></tr>
<tr><td>
 <textarea name="autoresp" cols="60" rows="20">
  <xsl:value-of select="autoresp"/>
 </textarea>
</td></tr>
<tr><td><input type="submit" value="Zmie�"/></td></tr>
<input type="hidden" name="id" value="autoresp"/>
<input type="hidden" name="conf" value="1"/>

<tr><td>
Ustawienie wiadomo�ci spowoduje, �e dla ka�dego nachodz�cego listu
generowana b�dzie automatyczna odpowied�. Informacje o nadawcach
s� przez pewien czas przechowywane co powoduje �e tylko jeden list
zostanie wys�any dla wiadomo�ci przychodz�cych w kr�tkim odst�pie czasu.
Odpowiedzi nie s� generowane dla list�w nadchodz�cych z list dyskusyjnych.
</td></tr>
</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
