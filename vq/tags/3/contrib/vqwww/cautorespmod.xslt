<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/autoresp">Automatyczna odpowiedź</xsl:template>
<xsl:template match="vqwww/mod/conf/mod/desc/autoresp">
Tutaj możesz ustawić odpowiedź, która będzie automatycznie wysyłana
do nadawców otrzymywanych wiadomości.
</xsl:template>

<xsl:template match="vqwww/mod/autoresp">
<form method="post">
<table border="0">

<xsl:choose>
<xsl:when test="getcant">
<tr><td>Nie można pobrać aktualnie ustawionej wiadomości.</td></tr>
</xsl:when>
<xsl:otherwise>

<!-- error messages -->
<xsl:if test="chgcant">
<tr><td colspan="2">Nie można zmienić wiadomości.</td></tr>
</xsl:if>

<!-- input form -->
<tr><td>Poniżej znajduje się treść odsyłanej wiadomości:</td></tr>
<tr><td>
 <textarea name="autoresp" cols="60" rows="20">
  <xsl:value-of select="autoresp"/>
 </textarea>
</td></tr>
<tr><td><input type="submit" value="Zmień"/></td></tr>
<input type="hidden" name="id" value="autoresp"/>
<input type="hidden" name="conf" value="1"/>

<tr><td>
Ustawienie wiadomości spowoduje, że dla każdego nachodzącego listu
generowana będzie automatyczna odpowiedź. Informacje o nadawcach
są przez pewien czas przechowywane co powoduje że tylko jeden list
zostanie wysłany dla wiadomości przychodzących w krótkim odstępie czasu.
Odpowiedzi nie są generowane dla listów nadchodzących z list dyskusyjnych.
</td></tr>
</xsl:otherwise>
</xsl:choose>

</table>
</form>
</xsl:template>

</xsl:stylesheet>
