<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="no" encoding="iso-8859-2"/>

<xsl:template match="vqsignup1_notify">X-Mailer: vqsignup1_notify, http://iidea.pl/~paweln/proj/vq/
Subject: Faktura proforma
From: admin@sarp.org.pl
Bcc: new@sarp.org.pl

Wystawca:
Stowarzyszenie Architektów Polskich
http://sarp.org.pl

Odbiorca:
Nazwa firmy: <xsl:value-of select="f_name"/>
NIP: <xsl:value-of select="f_nip"/>

Adres:
Ulica: <xsl:value-of select="f_str"/><xsl:text> </xsl:text><xsl:value-of select="f_house_no"/><xsl:text> </xsl:text><xsl:value-of select="f_app_no"/>
Miejscowość: <xsl:value-of select="f_city"/>
<xsl:if test="f_area/text()">
Województwo: <xsl:choose>
<xsl:when test="1">dolnośląskie</xsl:when>
<xsl:when test="2">kujawsko-pomorskie</xsl:when>
<xsl:when test="3">lubelskie</xsl:when>
<xsl:when test="4">lubuskie</xsl:when>
<xsl:when test="5">łódzkie</xsl:when>
<xsl:when test="6">małopolskie</xsl:when>
<xsl:when test="7">mazowieckie</xsl:when>
<xsl:when test="8">opolskie</xsl:when>
<xsl:when test="9">podkarpackie</xsl:when>
<xsl:when test="a">podlaskie</xsl:when>
<xsl:when test="b">pomorskie</xsl:when>
<xsl:when test="c">śląskie</xsl:when>
<xsl:when test="d">świętokrzyskie</xsl:when>
<xsl:when test="e">warmińsko-mazurskie</xsl:when>
<xsl:when test="f">wielkopolskie</xsl:when>
<xsl:when test="A">zachodniopomorskie</xsl:when>
</xsl:choose>
</xsl:if>
Kraj: <xsl:value-of select="f_country"/>
Kod pocztowy: <xsl:value-of select="f_pcode"/>

Dane konta:
Adres e-mail: <xsl:value-of select="new/user"/>@<xsl:value-of select="new/domain"/>
Data ważności: <xsl:value-of select="date_exp"/><xsl:text>
</xsl:text>
</xsl:template>
</xsl:stylesheet>
