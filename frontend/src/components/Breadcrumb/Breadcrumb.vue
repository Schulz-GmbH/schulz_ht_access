<template>
	<nav aria-label="Breadcrumb" role="navigation" class="breadcrumb">
		<ol class="flex flex-wrap pt-1 mr-12 bg-transparent rounded-lg sm:mr-16">
			<li class="breadcrumb-item leading-normal text-sm">
				<span class="opacity-50 text-slate-700">Pages</span>
			</li>
			<li v-for="(item, index) in breadcrumbs" :key="item.path"
				class="breadcrumb-item text-sm pl-2 capitalize leading-normal text-slate-700 before:float-left before:pr-2 before:text-gray-600 before:content-['/']"
				aria-current="page">
				<a v-if="index < breadcrumbs.length - 1" @click.prevent="handleLink(item)"
					class="breadcrumb-inner hover:underline">
					{{ item.meta.title }}
				</a>
				<span v-else class="breadcrumb-inner font-semibold">
					{{ item.meta.title }}
				</span>
			</li>
		</ol>

		<h6 class="mb-0 font-bold capitalize">
			{{ breadcrumbs[breadcrumbs.length - 1]?.meta.title || '' }}
		</h6>
	</nav>
</template>

<script lang="ts">

export { default } from './Breadcrumb'

</script>

<style src="./Breadcrumb.scss"></style>
