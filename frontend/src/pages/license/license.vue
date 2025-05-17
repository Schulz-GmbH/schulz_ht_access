<template>
	<div class="license p-6 max-w-3xl mx-auto">
		<h1 class="text-3xl font-bold mb-4">Lizenz & Nutzungshinweise</h1>
		<p class="mb-4">
			HS-Access ist urheberrechtlich geschützt. Diese Anwendung ist ausschließlich für
			den internen Gebrauch bestimmt. Jede Weiterverbreitung, Vervielfältigung,
			öffentliche Verfügung oder Bearbeitung – sei es elektronisch oder physisch –
			ist ohne ausdrückliche schriftliche Genehmigung untersagt.
		</p>
		<section class="conditions mb-6">
			<h2 class="text-2xl font-semibold mb-2">Erlaubte Nutzung</h2>
			<ul class="list-disc list-inside">
				<li>Installation und Betrieb innerhalb der Firma Schulz GmbH.</li>
				<li>Updates und Patches, die durch das Entwicklerteam bereitgestellt werden.</li>
			</ul>
		</section>
		<section class="prohibited">
			<h2 class="text-2xl font-semibold mb-2">Untersagte Handlungen</h2>
			<ul class="list-disc list-inside">
				<li>Weitergabe an Dritte jeglicher Art.</li>
				<li>Kopieren oder Modifizieren des Quellcodes.</li>
				<li>Reverse Engineering oder Dekompilierung.</li>
			</ul>
		</section>
		<p class="mt-6 text-sm text-gray-600">
			© 2025 HS-Access Team. Alle Rechte vorbehalten.
		</p>
	</div>
</template>

<script lang="ts">
export { default } from './license'

</script>

<style src="./license.scss" scoped></style>
